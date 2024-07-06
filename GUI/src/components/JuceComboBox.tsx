import React, { useEffect, useState } from "react";
// @ts-expect-error Juce does not have types
import * as Juce from "juce-framework-frontend";
import { Select } from "@radix-ui/themes";

interface JuceComboBoxProps {
  identifier: string;
  style: React.CSSProperties;
}

const JuceComboBox: React.FC<JuceComboBoxProps> = ({ identifier, style }) => {
  const comboBoxState = Juce.getComboBoxState(identifier);
  const [choiceIndex, setChoiceIndex] = useState(
    comboBoxState.getChoiceIndex()
  );
  const [properties, setProperties] = useState(comboBoxState.properties);

  const changeJUCEParamValue = (newIndex: number) => {
    comboBoxState.setChoiceIndex(newIndex);
    setChoiceIndex(newIndex);
  };

  useEffect(() => {
    const updateWebViewValue = () =>
      setChoiceIndex(comboBoxState.getChoiceIndex());
    const updateWebViewProperties = () =>
      setProperties(comboBoxState.properties);

    const valueListenerId =
      comboBoxState.valueChangedEvent.addListener(updateWebViewValue);
    const propertiesListenerId =
      comboBoxState.propertiesChangedEvent.addListener(updateWebViewProperties);

    return () => {
      comboBoxState.valueChangedEvent.removeListener(valueListenerId);
      comboBoxState.propertiesChangedEvent.removeListener(propertiesListenerId);
    };
  }, [comboBoxState]);

  const handleWheel = (event: React.WheelEvent<HTMLButtonElement>) => {
    if (event.deltaY < 0) {
      const newIndex =
        choiceIndex > 0 ? choiceIndex - 1 : properties.choices.length - 1;
      changeJUCEParamValue(newIndex);
    } else if (event.deltaY > 0) {
      const newIndex =
        choiceIndex < properties.choices.length - 1 ? choiceIndex + 1 : 0;
      changeJUCEParamValue(newIndex);
    }
  };

  return (
    <Select.Root
      value={properties.choices[choiceIndex]}
      onValueChange={(choice) => {
        const newIndex = properties.choices.indexOf(choice);
        changeJUCEParamValue(newIndex);
      }}
    >
      <Select.Trigger style={style} onWheel={handleWheel} />
      <Select.Content>
        {properties.choices.map((choice: string, index: number) => (
          <Select.Item key={index} value={choice}>
            {choice}
          </Select.Item>
        ))}
      </Select.Content>
    </Select.Root>
  );
};
export default JuceComboBox;
