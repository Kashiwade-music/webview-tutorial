/*
  ==============================================================================

        This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

//==============================================================================
/**
 */
class WebviewtutorialAudioProcessorEditor : public juce::AudioProcessorEditor,
                                            private juce ::Timer {
 public:
  WebviewtutorialAudioProcessorEditor(WebviewtutorialAudioProcessor &,
                                      juce::AudioProcessorValueTreeState &vts);
  ~WebviewtutorialAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

  void timerCallback() override;

 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  WebviewtutorialAudioProcessor &audioProcessor;

  // typedef juce::AudioProcessorValueTreeState::SliderAttachment
  // SliderAttachment; typedef
  // juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

  // juce::AudioProcessorValueTreeState &valueTreeState;
  // juce::Slider gainSlider;
  // std::unique_ptr<SliderAttachment> gainSliderAttachment;
  // juce::Slider panAngleSlider;
  // std::unique_ptr<SliderAttachment> panAngleSliderAttachment;
  // juce::ComboBox panRuleBox;
  // std::unique_ptr<ComboBoxAttachment> panRuleBoxAttachment;

  juce::WebSliderRelay gainRelay{webComponent, "gain"};
  juce::WebSliderParameterAttachment gainAttachment{
      *audioProcessor.parameters.getParameter("gain"), gainRelay, nullptr};
  juce::WebSliderRelay panAngleRelay{webComponent, "panangle"};
  juce::WebSliderParameterAttachment panAngleAttachment{
      *audioProcessor.parameters.getParameter("panangle"), panAngleRelay,
      nullptr};
  juce::WebComboBoxRelay panRuleBoxRelay{webComponent, "panrule"};
  juce::WebComboBoxParameterAttachment panRuleBoxAttachment{
      *audioProcessor.parameters.getParameter("panrule"), panRuleBoxRelay,
      nullptr};

  std::optional<juce::WebBrowserComponent::Resource> getResource(
      const juce::String &url);

  const char *getMimeForExtension(const juce::String &extension);

  juce::WebBrowserComponent webComponent{
      juce::WebBrowserComponent::Options{}
          .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
          .withWinWebView2Options(
              juce::WebBrowserComponent::Options::WinWebView2{}
                  .withUserDataFolder(juce::File::getSpecialLocation(
                      juce::File::SpecialLocationType::tempDirectory)))
          .withNativeIntegrationEnabled()
          .withOptionsFrom(gainRelay)
          .withOptionsFrom(panAngleRelay)
          .withOptionsFrom(panRuleBoxRelay)
          .withResourceProvider(
              [this](const auto &url) { return getResource(url); },
              juce::URL{"http://localhost:5173/"}.getOrigin())};

  std::deque<juce::Array<juce::var>> spectrumDataFrames;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      WebviewtutorialAudioProcessorEditor)
};
