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
  // numFramesBufferedは、バッファに保持するフレーム数を5に設定しています。
  static constexpr size_t numFramesBuffered = 5;

  // SpinLock::ScopedLockTypeを使ってロックを取得します。
  // これにより、processorRef.spectrumDataへのアクセスがスレッドセーフになります。
  juce::SpinLock::ScopedLockType lock{audioProcessor.spectrumDataLock};

  // frameという配列を作成し、processorRef.spectrumDataのデータをコピーします。
  // ただし、先頭の要素は除外しています（iが1から始まるため）。
  juce::Array<juce::var> frame;
  for (size_t i = 1; i < audioProcessor.spectrumData.size(); ++i)
    frame.add(audioProcessor.spectrumData[i]);

  // 作成したframeをstd::moveで spectrumDataFramesに追加します。
  //  このとき、ムーブセマンティクスを使用して効率的にデータを移動させています。
  spectrumDataFrames.push_back(std::move(frame));

  // spectrumDataFramesのサイズがnumFramesBufferedを超えた場合、古いフレームを削除します。
  while (spectrumDataFrames.size() > numFramesBuffered)
    spectrumDataFrames.pop_front();

  // コールバックの呼び出し回数をカウントするための変数を初期化します。
  static juce::int64 callbackCounter = 0;

  // spectrumDataFramesのサイズがnumFramesBufferedに達し、
  // かつcallbackCounterがnumFramesBufferedの倍数でない場合に、
  // ブラウザが可視状態であればspectrumDataイベントを送信します。
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
    // Array<var> 型の frames という配列を作成し、spectrumDataFrames 内の各
    // frame を frames に追加しています。spectrumDataFrames
    // は事前に定義されているスペクトルデータのフレームを保持する変数です。
    juce::Array<juce::var> frames;
    for (const auto &frame : spectrumDataFrames) frames.add(frame);

    // 新しい DynamicObject
    // を作成し、それをスマートポインタ（DynamicObject::Ptr）に格納しています。
    juce::DynamicObject::Ptr d(new juce::DynamicObject());

    // 動的オブジェクト d に対して2つのプロパティを設定しています。
    // "timeResolutionMs" プロパティにgetTimerInterval()の値を設定します。
    // この関数はタイマーの間隔をミリ秒単位で返すと推測されます。
    // "frames" プロパティに先ほど作成した frames
    // 配列を設定します。std::move(frames)を使って
    // framesの所有権を移動しています。
    d->setProperty("timeResolutionMs", getTimerInterval());
    d->setProperty("frames", std::move(frames));

    // 動的オブジェクト d を JSON 文字列に変換し、s という変数に格納しています。
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
