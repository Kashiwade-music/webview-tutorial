import { Flex } from "@radix-ui/themes";
import JuceSlider from "./components/JuceSlider";

function App() {
  return (
    <Flex justify="center" height="100px">
      <JuceSlider
        identifier="gain"
        title="Gain"
        defaultScaledValue={0.0}
        orientation={"vertical"}
      />
    </Flex>
  );
}

export default App;
