import React, { useEffect, useState } from "react";
// @ts-expect-error Juce does not have types
import * as Juce from "juce-framework-frontend";
import { P5CanvasInstance, ReactP5Wrapper } from "@p5-wrapper/react";

const sketch = (p5: P5CanvasInstance) => {
  p5.setup = () => p5.createCanvas(50, 300, p5.WEBGL);

  p5.draw = () => {
    p5.background(250);
    p5.normalMaterial();
    p5.push();
    p5.rotateZ(p5.frameCount * 0.01);
    p5.rotateX(p5.frameCount * 0.01);
    p5.rotateY(p5.frameCount * 0.01);
    p5.plane(100);
    p5.pop();
  };
};

interface AudioMeterProps {
  style?: React.CSSProperties;
}

const AudioMeter: React.FC<AudioMeterProps> = ({ style }) => {
  const [audioDataLeft, setAudioDataLeft] = useState([[0], [0], [0], [0], [0]]);
  const [audioDataRight, setAudioDataRight] = useState([
    [0],
    [0],
    [0],
    [0],
    [0],
  ]);
  const id = window.__JUCE__.backend.addEventListener("audioData", () => {
    fetch(Juce.getBackendResourceAddress("audioData.json"))
      .then((response) => response.text())
      .then((text) => {
        const data = JSON.parse(text);
        setAudioDataLeft(data.framesLeft);
        setAudioDataRight(data.framesRight);
      });
  });

  return <>{audioData}</>;
};

export default AudioMeter;
