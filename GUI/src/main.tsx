import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App.tsx";
import "@radix-ui/themes/styles.css";
import { Theme } from "@radix-ui/themes";
import "./global.css";

ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <Theme
      appearance="dark"
      panelBackground="translucent"
      hasBackground={false}
      style={{
        height: "100vh",
      }}
    >
      <App />
    </Theme>
  </React.StrictMode>
);
