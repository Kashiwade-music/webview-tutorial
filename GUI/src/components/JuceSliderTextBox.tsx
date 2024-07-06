import React, { useEffect, useState } from "react";
// @ts-expect-error Juce does not have types
import * as Juce from "juce-framework-frontend";
import { TextField } from "@radix-ui/themes";

interface JuceSliderTextBoxProps {
  identifier: string;
  digits: number;
  suffix?: string;
  style?: React.CSSProperties;
}

const JuceSliderTextBox: React.FC<JuceSliderTextBoxProps> = ({
  identifier,
  digits,
  suffix,
  style,
}) => {
  const sliderState = Juce.getSliderState(identifier);
  const [scaledValue, setScaledValue] = useState(sliderState.getScaledValue());
  const [scaledTempValue, setScaledTempValue] = useState(
    sliderState.getScaledValue().toFixed(digits)
  );
  const [properties, setProperties] = useState(sliderState.properties);
  const [isFocused, setIsFocused] = useState(false);

  const changeJUCEParamValue = (newNormalisedValue: number[]) => {
    sliderState.setNormalisedValue(newNormalisedValue[0]);
  };

  useEffect(() => {
    const updateWebViewValue = () => {
      setScaledTempValue(sliderState.getScaledValue().toFixed(digits));
      setScaledValue(sliderState.getScaledValue());
    };
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
  }, [sliderState, digits]);

  return (
    <TextField.Root
      value={
        isFocused
          ? scaledTempValue
          : suffix
          ? `${scaledValue.toFixed(digits)} ${suffix}`
          : scaledValue.toFixed(digits)
      }
      onChange={(event) => {
        setScaledTempValue(event.target.value);
      }}
      style={style}
      onFocus={(event) => {
        setScaledTempValue(scaledValue.toFixed(digits));
        setIsFocused(true);
        setTimeout(() => {
          event.target.select();
        }, 100);
      }}
      onBlur={() => {
        const newValue = parseFloat(scaledTempValue);
        if (!isNaN(newValue)) {
          changeJUCEParamValue([
            Math.pow(
              (newValue - sliderState.properties.start) /
                (sliderState.properties.end - sliderState.properties.start),
              sliderState.properties.skew
            ),
          ]);
        }
        setIsFocused(false);
      }}
      onKeyDown={(event) => {
        if (event.key === "Enter") {
          event.currentTarget.blur();
        }
      }}
    />
  );
};

export default JuceSliderTextBox;
