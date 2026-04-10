'''
Statyczny prototyp wtyczki z dynamiczną detekcją częstotliwości
- przetwarza sygnał z pliku wav po jednym buforze
  na raz, generuje falę sinusoidalną na podstawie
  częstotliwości dominującej, zapisuje wygenerowaną
  falę w pliku
- wykorzystuje generator z kontynuacją fazy, aby uniknąć
  artefaktów (klików) w sygnale wyjściowym


Użycie:
- domyślne:
    python3 main.py <ścieżka_pliku_wejściowego>

- lub ze specyfikacją ścieżki wyjściowej:
    python3 main.py <ścieżka_pliku_wejściowego> -o <ścieżka_pliku_wyjściowego>

- lub z własnym rozmiarem bufora:
    python3 main.py <ścieżka_pliku_wejściowego> -b <rozmiar_bufora>
'''

import numpy as np
import argparse
import sys
import os
import soundfile as sf


# Constants
BUFFER_SIZE = 1024


class ContinuousSineGenerator:
    """
    Generator fali sinusoidalnej z pamięcią fazy.
    Zapewnia ciągłość sygnału przy zmianach częstotliwości.
    """
    
    def __init__(self, sample_rate):
        """
        Inicjalizacja generatora.
        
        Args:
            sample_rate: Częstotliwość próbkowania w Hz
        """
        self.sample_rate = sample_rate
        self.current_phase = 0.0        # Aktualna faza w radianach
        self.current_frequency = 0.0    # Aktualna częstotliwość w Hz
        self.last_sample = 0.0          # Ostatnia wygenerowana próbka
        
    def generate_buffer(self, frequency, num_samples, amplitude=0.5):
        """
        Generuje bufor sygnału sinusoidalnego z ciągłością fazy.
        
        Args:
            frequency: Nowa częstotliwość w Hz
            num_samples: Liczba próbek do wygenerowania
            amplitude: Amplituda sygnału (zakres 0-1)
            
        Returns:
            Tablica numpy float32 z próbkami sinusa (zakres [-1, 1])
        """
        if num_samples <= 0:
            return np.array([], dtype=np.float32)
        
        # Jeśli częstotliwość jest zerowa lub poza zakresem, generuj ciszę
        if frequency == 0 or frequency > self.sample_rate / 2:
            # Resetuj fazę tylko jeśli przechodzimy w ciszę
            if self.current_frequency != 0:
                self.current_phase = 0.0
                self.current_frequency = 0.0
                self.last_sample = 0.0
            return np.zeros(num_samples, dtype=np.float32)
        
        # Oblicz przyrost fazy dla nowej częstotliwości
        phase_increment = 2 * np.pi * frequency / self.sample_rate
        
        # Generuj próbki z ciągłą fazą
        samples = np.zeros(num_samples, dtype=np.float32)
        phase = self.current_phase
        
        for i in range(num_samples):
            # Generuj próbkę
            samples[i] = amplitude * np.sin(phase)
            
            # Aktualizuj fazę
            phase += phase_increment
            
            # Normalizuj fazę do zakresu [0, 2π)
            if phase >= 2 * np.pi:
                phase -= 2 * np.pi
            elif phase < 0:
                phase += 2 * np.pi
        
        # Zapamiętaj końcową fazę i ostatnią próbkę dla następnego bufora
        self.current_phase = phase
        self.current_frequency = frequency
        self.last_sample = samples[-1] if num_samples > 0 else 0.0
        
        # Ogranicz do zakresu [-1, 1]
        samples = np.clip(samples, -1.0, 1.0)
        
        return samples.astype(np.float32)
    
    def reset(self):
        """Resetuje stan generatora."""
        self.current_phase = 0.0
        self.current_frequency = 0.0
        self.last_sample = 0.0


class AudioProcessor:
    def __init__(self, buffer_size=BUFFER_SIZE):
        """
        Inicjalizacja procesora audio.
        
        Args:
            buffer_size: Rozmiar buforu w próbkach
        """
        self.buffer_size = buffer_size
    
    def read_wav(self, file_path):
        """
        Odczytuje plik WAV
        
        Returns:
            (data, sample_rate): data jako tablica numpy (float32) i częstotliwość próbkowania
        """
        # soundfile automatycznie obsługuje różne formaty
        data, sample_rate = sf.read(file_path)
        
        # Jeśli stereo lub więcej kanałów, weź tylko pierwszy kanał
        if len(data.shape) > 1:
            data = data[:, 0]  # Weź lewy kanał
        
        # Upewnij się, że dane są float32 w zakresie [-1, 1]
        if data.dtype == np.int16:
            # Dla int16 zakres to [-32768, 32767]
            max_value = np.iinfo(np.int16).max
            data = data.astype(np.float32) / max_value
        elif data.dtype == np.int32:
            # Dla int32 zakres to [-2147483648, 2147483647]
            max_value = np.iinfo(np.int32).max
            data = data.astype(np.float32) / max_value
        elif data.dtype == np.float64:
            # Dla float64 wystarczy konwersja do float32
            data = data.astype(np.float32)

        return data, sample_rate
    
    def write_wav(self, file_path, data, sample_rate):
        """
        Zapisuje dane do pliku WAV.
        Data powinna być w formacie float32 z zakresu [-1, 1].
        """
        # Upewnij się, że dane są w zakresie [-1, 1]
        data = np.clip(data, -1.0, 1.0)
        
        # Zapisz jako 32-bit float dla najlepszej jakości
        sf.write(file_path, data, sample_rate, subtype='FLOAT')
    
    def find_dominant_frequency(self, buffer, sample_rate):
        """
        Znajduje dominującą częstotliwość w buforze używając FFT.
        
        Returns:
            Dominująca częstotliwość w Hz
        """
        # Zastosuj okno Hanninga aby zredukować wycieki widma
        windowed_buffer = buffer * np.hanning(len(buffer))
        
        # Wykonaj FFT
        fft_result = np.fft.fft(windowed_buffer)
        magnitude = np.abs(fft_result[:len(fft_result)//2])
        
        # Znajdź indeks z największą amplitudą (pomijając DC)
        if len(magnitude) > 1:
            # Pomijamy pierwsze kilka binów (DC i bardzo niskie częstotliwości)
            start_idx = 2
            if start_idx < len(magnitude):
                dominant_index = np.argmax(magnitude[start_idx:]) + start_idx
            else:
                dominant_index = np.argmax(magnitude[1:]) + 1
        else:
            dominant_index = 0
        
        # Oblicz częstotliwość
        if dominant_index > 0 and dominant_index < len(magnitude):
            frequency = dominant_index * sample_rate / self.buffer_size
        else:
            frequency = 0.0
        
        return frequency
    
    def process_file(self, input_path, output_path):
        """
        Główna funkcja przetwarzająca plik.
        
        Args:
            input_path: Ścieżka do pliku wejściowego
            output_path: Ścieżka do pliku wyjściowego
        """
        print(f"Odczytuję plik: {input_path}")
        
        # Sprawdź czy plik istnieje
        if not os.path.exists(input_path):
            raise FileNotFoundError(f"Plik {input_path} nie istnieje")
        
        # Odczytaj plik wejściowy
        data, sample_rate = self.read_wav(input_path)
        
        # Inicjalizuj generator ciągły
        generator = ContinuousSineGenerator(sample_rate)
        
        print(f"Częstotliwość próbkowania: {sample_rate} Hz")
        print(f"Rozmiar buforu: {self.buffer_size} próbek")
        print(f"Całkowita liczba próbek: {len(data)}")
        print(f"Czas trwania: {len(data)/sample_rate:.2f} sekundy")
        print(f"Zakres danych: [{data.min():.3f}, {data.max():.3f}]")
        
        # Przygotuj tablicę dla wynikowego sygnału
        num_buffers = (len(data) + self.buffer_size - 1) // self.buffer_size
        output_data = np.zeros(num_buffers * self.buffer_size, dtype=np.float32)
        
        # Zmienna do przechowywania poprzedniej częstotliwości (dla info)
        prev_freq = 0.0
        
        # Przetwarzaj bufor po buforze
        for i in range(num_buffers):
            start_idx = i * self.buffer_size
            end_idx = min(start_idx + self.buffer_size, len(data))
            
            # Pobierz bufor
            buffer = data[start_idx:end_idx]
            
            # Jeśli bufor jest krótszy, uzupełnij zerami
            if len(buffer) < self.buffer_size:
                buffer = np.pad(buffer, (0, self.buffer_size - len(buffer)), 'constant')
            
            # Znajdź dominującą częstotliwość
            dominant_freq = self.find_dominant_frequency(buffer, sample_rate)
            
            # Oblicz amplitudę na podstawie RMS oryginalnego buforu
            rms_amplitude = np.sqrt(np.mean(buffer**2))
            amplitude = min(rms_amplitude * 1.5, 0.9)  # Zachowaj podobną głośność
            
            # Generuj falę sinusoidalną z ciągłością fazy
            sine_wave = generator.generate_buffer(dominant_freq, self.buffer_size, amplitude)
            
            # Zapisz do wyjściowej tablicy
            output_start = i * self.buffer_size
            output_end = output_start + self.buffer_size
            output_data[output_start:output_end] = sine_wave
        
        # Przytnij do oryginalnej długości
        output_data = output_data[:len(data)]
        
        # Zapisz plik wyjściowy
        self.write_wav(output_path, output_data, sample_rate)
        print(f"\nZapisano przetworzony plik: {output_path}")
        
        # Podsumowanie
        print(f"\nPodsumowanie:")
        print(f"  Plik wejściowy: {input_path}")
        print(f"  Plik wyjściowy: {output_path}")
        print(f"  Rozmiar bufora: {self.buffer_size} próbek")
        print(f"  Liczba buforów: {num_buffers}")

def main():
    parser = argparse.ArgumentParser(
        description='Przetwarzanie audio - zamiana na sygnały sinusoidalne z ciągłością fazy',
        epilog='Przykład: python main.py input.wav -o output.wav -b 2048'
    )
    parser.add_argument('input_file', help='Ścieżka do wejściowego pliku WAV')
    parser.add_argument('-o', '--output', default='output.wav', 
                       help='Ścieżka do wyjściowego pliku WAV (domyślnie: output.wav)')
    parser.add_argument('-b', '--buffer', type=int, default=1024,
                       help='Rozmiar buforu w próbkach (domyślnie: 1024)')
    
    args = parser.parse_args()
    
    try:
        processor = AudioProcessor(buffer_size=args.buffer)
        processor.process_file(args.input_file, args.output)
        print("\nPrzetwarzanie zakończone pomyślnie.")
    except FileNotFoundError as e:
        print(f"\nBłąd: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"\nBłąd podczas przetwarzania: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
