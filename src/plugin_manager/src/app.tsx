import styled from "styled-components";
import * as React from "react";
import { createRoot } from "react-dom/client";

const Container = styled.div`
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100%;
`;

const Button = styled.button`
  width: 250px;
  height: 250px;
  border-radius: 99999px;
  background-color: red;

  &:hover {
    background-color: blue;
  }
`;

function onClick() {
  console.log("JS BUTTON CLICKED");
  // console.log(new Q.Object());
  // console.log(new Q.PushButton());
  // console.log(new K.Krita());
}

function App() {
  return (
    <Container>
      <Button onClick={onClick}>Click me!</Button>
    </Container>
  );
}

const rootElement = document.createElement("div");
rootElement.style.height = "100%";
document.body.append(rootElement);
const root = createRoot(rootElement);
root.render(<App />);
