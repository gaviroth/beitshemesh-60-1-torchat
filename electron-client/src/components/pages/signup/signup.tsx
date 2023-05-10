import { useState } from "react";
import { useNavigate } from "react-router-dom";
import { InputGroup, Form, Button, Alert } from "react-bootstrap";
import { ipcRenderer } from "electron";

export default function SignUp() {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [email, setEmail] = useState("");
  const [alert, setAlert] = useState("");
  const [locked, setLocked] = useState(false);
  const navigate = useNavigate();

  const signUp = () => {
    setLocked(true);
    const data = {
      Username: username,
      Password: password,
      Email: email,
    };
    ipcRenderer.send("signUp", data);
  };

  ipcRenderer.on("signUpSuccess", (event, arg: any) => {
    localStorage.setItem("username", username);
    localStorage.setItem("token", arg.token);
    setLocked(false);
    navigate("/home");
  });

  ipcRenderer.on("serverError", (event, arg: any) => {
    setAlert(arg.error);
    setLocked(false);
  });

  const clearAlert = () => {
    setAlert("");
  };

  return (
    <>
      {alert && (
        <Alert variant="danger" onClose={clearAlert} dismissible>
          {alert}
        </Alert>
      )}
      <InputGroup>
        <Form.Control
          type="text"
          placeholder="Username"
          onChange={(e) => setUsername(e.target.value)}
          disabled={locked}
        />
      </InputGroup>
      <InputGroup>
        <Form.Control
          type="password"
          placeholder="Password"
          onChange={(e) => setPassword(e.target.value)}
          disabled={locked}
        />
      </InputGroup>
      <InputGroup>
        <Form.Control
          type="email"
          placeholder="Email"
          onChange={(e) => setEmail(e.target.value)}
          disabled={locked}
        />
      </InputGroup>

      <Button variant="primary" onClick={signUp} disabled={locked}>
        Sign up!
      </Button>
      <Button
        variant="secondary"
        onClick={() => navigate("/")}
        disabled={locked}
      >
        {" "}
        Login
      </Button>
    </>
  );
}
