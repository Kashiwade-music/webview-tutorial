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

  webComponent.goToURL("http://localhost:5173/");
  // webComponent.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

  setSize(170, 650);
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

void WebviewtutorialAudioProcessorEditor::timerCallback() {
  // numFramesBuffered�́A�o�b�t�@�ɕێ�����t���[������5�ɐݒ肵�Ă��܂��B
  static constexpr size_t numFramesBuffered = 5;

  // SpinLock::ScopedLockType���g���ă��b�N���擾���܂��B
  // ����ɂ��AprocessorRef.spectrumData�ւ̃A�N�Z�X���X���b�h�Z�[�t�ɂȂ�܂��B
  juce::SpinLock::ScopedLockType lock{audioProcessor.spectrumDataLock};

  // frame�Ƃ����z����쐬���AprocessorRef.spectrumData�̃f�[�^���R�s�[���܂��B
  // �������A�擪�̗v�f�͏��O���Ă��܂��ii��1����n�܂邽�߁j�B
  juce::Array<juce::var> frame;
  for (size_t i = 1; i < audioProcessor.spectrumData.size(); ++i)
    frame.add(audioProcessor.spectrumData[i]);

  // �쐬����frame��std::move�� spectrumDataFrames�ɒǉ����܂��B
  //  ���̂Ƃ��A���[�u�Z�}���e�B�N�X���g�p���Č����I�Ƀf�[�^���ړ������Ă��܂��B
  spectrumDataFrames.push_back(std::move(frame));

  // spectrumDataFrames�̃T�C�Y��numFramesBuffered�𒴂����ꍇ�A�Â��t���[�����폜���܂��B
  while (spectrumDataFrames.size() > numFramesBuffered)
    spectrumDataFrames.pop_front();

  // �R�[���o�b�N�̌Ăяo���񐔂��J�E���g���邽�߂̕ϐ������������܂��B
  static juce::int64 callbackCounter = 0;

  // spectrumDataFrames�̃T�C�Y��numFramesBuffered�ɒB���A
  // ����callbackCounter��numFramesBuffered�̔{���łȂ��ꍇ�ɁA
  // �u���E�U������Ԃł����spectrumData�C�x���g�𑗐M���܂��B
  if (spectrumDataFrames.size() == numFramesBuffered &&
      callbackCounter++ % (juce::int64)numFramesBuffered) {
    webComponent.emitEventIfBrowserIsVisible("spectrumData", juce::var{});
  }
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

  if (urlToRetrive == "audioData.json") {
    // Array<var> �^�� frames �Ƃ����z����쐬���AspectrumDataFrames ���̊e
    // frame �� frames �ɒǉ����Ă��܂��BspectrumDataFrames
    // �͎��O�ɒ�`����Ă���X�y�N�g���f�[�^�̃t���[����ێ�����ϐ��ł��B
    juce::Array<juce::var> frames;
    for (const auto &frame : spectrumDataFrames) frames.add(frame);

    // �V���� DynamicObject
    // ���쐬���A������X�}�[�g�|�C���^�iDynamicObject::Ptr�j�Ɋi�[���Ă��܂��B
    juce::DynamicObject::Ptr d(new juce::DynamicObject());

    // ���I�I�u�W�F�N�g d �ɑ΂���2�̃v���p�e�B��ݒ肵�Ă��܂��B
    // "timeResolutionMs" �v���p�e�B��getTimerInterval()�̒l��ݒ肵�܂��B
    // ���̊֐��̓^�C�}�[�̊Ԋu���~���b�P�ʂŕԂ��Ɛ�������܂��B
    // "frames" �v���p�e�B�ɐ�قǍ쐬���� frames
    // �z���ݒ肵�܂��Bstd::move(frames)���g����
    // frames�̏��L�����ړ����Ă��܂��B
    d->setProperty("timeResolutionMs", getTimerInterval());
    d->setProperty("frames", std::move(frames));

    // ���I�I�u�W�F�N�g d �� JSON ������ɕϊ����As �Ƃ����ϐ��Ɋi�[���Ă��܂��B
    const auto s = juce::JSON::toString(d.get());
    juce::MemoryInputStream stream{s.getCharPointer(), s.getNumBytesAsUTF8(),
                                   false};

    std::vector<std::byte> result((size_t)stream.getTotalLength());
    stream.setPosition(0);
    stream.read(result.data(), result.size());
    return juce::WebBrowserComponent::Resource{
        std::move(result), juce::String{"application/json"}};
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
