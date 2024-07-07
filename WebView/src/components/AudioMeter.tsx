import React, { useEffect, useState } from "react";
// @ts-expect-error Juce does not have types
import * as Juce from "juce-framework-frontend";
import { P5CanvasInstance, ReactP5Wrapper } from "@p5-wrapper/react";

interface AudioMeterProps {
  style?: React.CSSProperties;
}

const AudioMeter: React.FC<AudioMeterProps> = ({ style }) => {
  let audioDataLeft = [[0], [0], [0], [0], [0]];
  let audioDataRight = [[0], [0], [0], [0], [0]];

  const getNormalisedValue = (decibel: number) => {
    const start = -100;
    const end = 10;
    const skew = 2;
    return Math.pow((decibel - start) / (end - start), skew);
  };

  const sketch = (p5: P5CanvasInstance) => {
    p5.setup = () => p5.createCanvas(25, 300, p5.WEBGL);

    p5.draw = () => {
      p5.background(250);
      p5.fill("#3358D4");
      p5.noStroke();
      p5.rect(
        -p5.width / 2,
        p5.height * (0.5 - getNormalisedValue(audioDataLeft[4][0])),
        p5.width / 2,
        getNormalisedValue(audioDataLeft[4][0]) * p5.height
      );
      p5.rect(
        0,
        p5.height * (0.5 - getNormalisedValue(audioDataRight[4][0])),
        p5.width / 2,
        getNormalisedValue(audioDataRight[4][0]) * p5.height
      );
    };
  };

  const id = window.__JUCE__.backend.addEventListener("audioData", () => {
    fetch(Juce.getBackendResourceAddress("audioData.json"))
      .then((response) => response.text())
      .then((text) => {
        const data = JSON.parse(text);
        audioDataLeft = data.framesLeft;
        audioDataRight = data.framesRight;
      });
  });

  return (
    <>
      <ReactP5Wrapper sketch={sketch} />
    </>
  );
};

export default AudioMeter;
