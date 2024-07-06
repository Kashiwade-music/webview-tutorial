import { Flex } from "@radix-ui/themes";
import JuceSlider from "./components/JuceSlider";
import JuceSliderTextBox from "./components/JuceSliderTextBox";
import JuceComboBox from "./components/JuceComboBox";

function App() {
  return (
    <Flex justify="center" direction={"column"} align={"center"}>
      <JuceSlider
        identifier="gain"
        defaultScaledValue={0.0}
        orientation={"vertical"}
        style={{
          height: "300px",
          width: "50px",
        }}
      />
      <JuceSliderTextBox identifier="gain" digits={2} suffix="dB" />
      <JuceSlider
        identifier="panangle"
        defaultScaledValue={0.0}
        orientation={"horizontal"}
        style={{
          height: "50px",
          width: "300px",
        }}
      />
      <JuceSliderTextBox identifier="panangle" digits={2} />
      <JuceComboBox
        identifier="panrule"
        style={{
          height: "30px",
          width: "200px",
        }}
      />
    </Flex>
  );
}

export default App;
