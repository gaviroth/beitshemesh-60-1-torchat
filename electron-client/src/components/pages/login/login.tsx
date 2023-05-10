import { Form, Button, InputGroup, Alert } from "react-bootstrap";
import { useState } from "react";
import { useNavigate } from "react-router-dom";
import SignUp from "../signup/signup";
import { ipcRenderer } from "electron";

export default function Login() {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [locked, setLocked] = useState(false);
  const [alert, setAlert] = useState("");

  const navigate = useNavigate();

  const handleUsernameChange = (e: any) => {
    setUsername(e.target.value);
  };

  const handlePasswordChange = (e: any) => {
    setPassword(e.target.value);
  };

  const handleLogin = () => {
    setLocked(true);
    const data = {
      username: username,
      password: password,
    };
    ipcRenderer.send("login", data);
  };

  const handleSignup = () => {
    navigate("/signup");
  };

  ipcRenderer.on("loginReact", (event, arg: any) => {
    localStorage.setItem("token", arg.token);
    localStorage.setItem("username", username);
    setLocked(false);
    navigate("/home");
  });

  ipcRenderer.on("serverError", (event, arg: any) => {
    setLocked(false);
    setAlert(arg.error);
  });

  return (
    <>
      {alert && (
        <Alert variant="danger" onClose={() => setAlert("")} dismissible>
          {alert}
        </Alert>
      )}
      <InputGroup>
        <Form.Control
          type="text"
          placeholder="Username"
          onChange={handleUsernameChange}
          disabled={locked}
        />
      </InputGroup>
      <InputGroup>
        <Form.Control
          type="password"
          placeholder="Password"
          onChange={handlePasswordChange}
          disabled={locked}
        />
      </InputGroup>
      <Button variant="primary" onClick={handleLogin} disabled={locked}>
        Login!
      </Button>
      <Button variant="secondary" onClick={handleSignup} disabled={locked}>
        Sign up
      </Button>
    </>
  );
}
