import React, { useEffect, useState, useRef } from "react";
// @ts-expect-error Juce does not have types
import * as Juce from "juce-framework-frontend";
import { Flex, Slider } from "@radix-ui/themes";

interface JuceSliderProps {
  identifier: string;
  title: string;
  defaultScaledValue: number;
  orientation: "vertical" | "horizontal";
}

const JuceSlider: React.FC<JuceSliderProps> = ({
  identifier,
  title,
  defaultScaledValue,
  orientation,
}) => {
  const sliderState = Juce.getSliderState(identifier);
  const defaultNomalisedValue = Math.pow(
    (defaultScaledValue - sliderState.properties.start) /
      (sliderState.properties.end - sliderState.properties.start),
    sliderState.properties.skew
  );
  const [value, setValue] = useState(sliderState.getNormalisedValue());
  const [properties, setProperties] = useState(sliderState.properties);

  const handleChange = (newValue: number[]) => {
    sliderState.setNormalisedValue(newValue[0]);
    setValue(newValue);
  };

  useEffect(() => {
    const updateValue = () => setValue(sliderState.getNormalisedValue());
    const updateProperties = () => setProperties(sliderState.properties);

    const valueListenerId =
      sliderState.valueChangedEvent.addListener(updateValue);
    const propertiesListenerId =
      sliderState.propertiesChangedEvent.addListener(updateProperties);

    return () => {
      sliderState.valueChangedEvent.removeListener(valueListenerId);
      sliderState.propertiesChangedEvent.removeListener(propertiesListenerId);
    };
  }, [sliderState]);

  const handleClick = (event: React.MouseEvent<HTMLDivElement, MouseEvent>) => {
    if (event.ctrlKey) {
      handleChange([defaultNomalisedValue]);
    }
  };

  const handleWheel = (event: React.WheelEvent<HTMLDivElement>) => {
    const stepSize = event.shiftKey ? 0.00005 : 0.0002;
    const clampedValue = Math.min(
      Math.max(value - event.deltaY * stepSize, 0),
      1
    );
    handleChange([clampedValue]);
  };

  return (
    <Slider
      defaultValue={[value]}
      value={[value]}
      orientation={orientation}
      size="1"
      onWheel={handleWheel}
      onClick={handleClick}
      onValueChange={handleChange}
      min={0}
      max={1}
      step={0.001}
      style={{
        height: "300px",
      }}
    />
  );
};

export default JuceSlider;
