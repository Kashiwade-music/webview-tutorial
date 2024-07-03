/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

//==============================================================================
WebviewtutorialAudioProcessorEditor::WebviewtutorialAudioProcessorEditor(
    WebviewtutorialAudioProcessor &p, juce::AudioProcessorValueTreeState &vts)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  addAndMakeVisible(webComponent);
  // webComponent.goToURL("http://localhost:5173/");
  webComponent.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
  setSize(500, 500);
}

WebviewtutorialAudioProcessorEditor::~WebviewtutorialAudioProcessorEditor() {}

//==============================================================================
void WebviewtutorialAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void WebviewtutorialAudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  webComponent.setBounds(getLocalBounds());
}

//==============================================================================
std::optional<juce::WebBrowserComponent::Resource>
WebviewtutorialAudioProcessorEditor::getResource(const juce::String &url) {
  const auto urlToRetrive = url == "/"
                                ? juce::String{"index.html"}
                                : url.fromFirstOccurrenceOf("/", false, false);

  static auto stream = juce::File(
                           "C:/Users/ryo/PlainTextProject/backup-via-Git/"
                           "webview-tutorial/GUI/dist.zip")
                           .createInputStream();

  if (stream != nullptr) {
    static juce::ZipFile archive{stream.get(), false};
    DBG("archive size: " << archive.getNumEntries());
    DBG("url: " << urlToRetrive);
    DBG("test_entry_index: " << archive.getIndexOfFileName(urlToRetrive));
    DBG("file name of index=1: " << archive.getEntry(1)->filename);

    if (auto *entry = archive.getEntry(urlToRetrive)) {
      DBG("entry: " << entry->filename);
      auto entryStream = rawToUniquePtr(archive.createStreamForEntry(*entry));
      std::vector<std::byte> result((size_t)entryStream->getTotalLength());
      entryStream->setPosition(0);
      entryStream->read(result.data(), result.size());

      auto mime = getMimeForExtension(
          entry->filename.fromLastOccurrenceOf(".", false, false)
              .toLowerCase());
      DBG("mime: " << mime);
      return juce::WebBrowserComponent::Resource{std::move(result),
                                                 std::move(mime)};
    }
  }

  return std::nullopt;
}

const char *WebviewtutorialAudioProcessorEditor::getMimeForExtension(
    const juce::String &extension) {
  static const std::unordered_map<juce::String, const char *> mimeMap = {
      {{"htm"}, "text/html"},
      {{"html"}, "text/html"},
      {{"txt"}, "text/plain"},
      {{"jpg"}, "image/jpeg"},
      {{"jpeg"}, "image/jpeg"},
      {{"svg"}, "image/svg+xml"},
      {{"ico"}, "image/vnd.microsoft.icon"},
      {{"json"}, "application/json"},
      {{"png"}, "image/png"},
      {{"css"}, "text/css"},
      {{"map"}, "application/json"},
      {{"js"}, "text/javascript"},
      {{"woff2"}, "font/woff2"}};

  if (const auto it = mimeMap.find(extension.toLowerCase());
      it != mimeMap.end())
    return it->second;

  jassertfalse;
  return "";
}
