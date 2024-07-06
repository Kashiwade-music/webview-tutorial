import React, { useEffect, useState } from "react";
// @ts-expect-error Juce does not have types
import * as Juce from "juce-framework-frontend";
import { Slider } from "@radix-ui/themes";

interface JuceSliderProps {
  identifier: string;
  defaultScaledValue: number;
  orientation: "vertical" | "horizontal";
  style: React.CSSProperties;
}

const JuceSlider: React.FC<JuceSliderProps> = ({
  identifier,
  defaultScaledValue,
  orientation,
  style,
}) => {
  const sliderState = Juce.getSliderState(identifier);
  const defaultNomalisedValue = Math.pow(
    (defaultScaledValue - sliderState.properties.start) /
      (sliderState.properties.end - sliderState.properties.start),
    sliderState.properties.skew
  );
  const [value, setValue] = useState(sliderState.getNormalisedValue());
  const [properties, setProperties] = useState(sliderState.properties);

  const changeJUCEParamValue = (newNormalisedValue: number[]) => {
    sliderState.setNormalisedValue(newNormalisedValue[0]);
    setValue(newNormalisedValue[0]);
  };

  useEffect(() => {
    const updateWebViewValue = () => setValue(sliderState.getNormalisedValue());
    const updateWebViewProperties = () => setProperties(sliderState.properties);

    const valueListenerId =
      sliderState.valueChangedEvent.addListener(updateWebViewValue);
    const propertiesListenerId = sliderState.propertiesChangedEvent.addListener(
      updateWebViewProperties
    );

    return () => {
      sliderState.valueChangedEvent.removeListener(valueListenerId);
      sliderState.propertiesChangedEvent.removeListener(propertiesListenerId);
    };
  }, [sliderState]);

  const handleClick = (event: React.MouseEvent<HTMLDivElement, MouseEvent>) => {
    if (event.ctrlKey) {
      changeJUCEParamValue([defaultNomalisedValue]);
    }
  };

  const handleWheel = (event: React.WheelEvent<HTMLDivElement>) => {
    const stepSize = event.shiftKey ? 0.00005 : 0.0002;
    const clampedValue = Math.min(
      Math.max(value - event.deltaY * stepSize, 0),
      1
    );
    changeJUCEParamValue([clampedValue]);
  };

  return (
    <>
      <Slider
        defaultValue={[value]}
        value={[value]}
        orientation={orientation}
        size="1"
        onWheel={handleWheel}
        onClick={handleClick}
        onValueChange={changeJUCEParamValue}
        min={0}
        max={1}
        step={0.001}
        style={style}
      />
    </>
  );
};

export default JuceSlider;
