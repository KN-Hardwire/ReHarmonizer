/* Temporary file for testing */

#include <juce_audio_processors/juce_audio_processors.h>
#include <iostream>

class PluginSetupTest : public juce::AudioProcessor
{
public:
	PluginSetupTest()
	{
		std::cout << "PluginSetupTest: object created." << std::endl;
	}
	~PluginSetupTest() override
	{
		std::cout << "PluginSetupTest: object destroyed. Test completed." << std::endl;
	}

	void prepareToPlay(double /* sampleRate */, int /* samplesPerBlock */) override
	{
		std::cout << "PluginSetupTest: prepareToPlay called." << std::endl;
	}

	void releaseResources() override {}

	void processBlock(juce::AudioBuffer<float>& /* buffer */, juce::MidiBuffer& /* midiMessages */) override
	{
		std::cout << "PluginSetupTest: processBlock called." << std::endl;
	}

	// Boilerplate overrides
	juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    const juce::String getName() const override { return "MinimalPlugin"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
};

// Plugin entry point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new PluginSetupTest();
}
