import * as React from "react";
import { createRoot } from "react-dom/client";

function onClick() {
  console.log("JS BUTTON CLICKED");
  const a = new MyClass();
  console.log(a);
  console.log(typeof MyClass);
  console.log(typeof a);
}

function App() {
  return <button onClick={onClick}>Click me!</button>;
}

const root = createRoot(document.getElementById("root")!);
root.render(<App />);
