import { Flex, Card, Heading } from "@radix-ui/themes";
import JuceSlider from "./components/JuceSlider";
import JuceSliderTextBox from "./components/JuceSliderTextBox";
import JuceComboBox from "./components/JuceComboBox";

function App() {
  return (
    <Flex
      justify="center"
      direction={"column"}
      align={"center"}
      style={{ height: "100%" }}
    >
      <Flex
        justify="center"
        direction={"column"}
        align={"center"}
        gap={"3"}
        m={"3"}
      >
        <Card>
          <Flex
            justify="center"
            direction={"column"}
            align={"center"}
            gap={"3"}
          >
            <Heading>Pan</Heading>

            <JuceSlider
              identifier="panangle"
              defaultScaledValue={0.0}
              orientation={"horizontal"}
              style={{
                height: "50px",
                width: "120px",
              }}
            />
            <JuceSliderTextBox
              identifier="panangle"
              digits={2}
              style={{ textAlign: "center", width: "120px" }}
            />
            <JuceComboBox
              identifier="panrule"
              style={{
                height: "30px",
                width: "120px",
              }}
            />
          </Flex>
        </Card>
        <Card>
          <Flex
            justify="center"
            direction={"column"}
            align={"center"}
            gap={"3"}
          >
            <Heading>Gain</Heading>
            <JuceSlider
              identifier="gain"
              defaultScaledValue={0.0}
              orientation={"vertical"}
              style={{
                height: "300px",
                width: "50px",
              }}
            />
            <JuceSliderTextBox
              identifier="gain"
              digits={2}
              suffix="dB"
              style={{ textAlign: "center", width: "120px" }}
            />
          </Flex>
        </Card>
      </Flex>
    </Flex>
  );
}

export default App;
