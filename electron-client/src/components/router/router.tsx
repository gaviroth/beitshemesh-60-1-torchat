import { BrowserRouter, Route, Routes } from "react-router-dom";
import Login from "../pages/login/login";
import Signup from "../pages/signup/signup";
import Home from "../pages/home/Home";

export default function Router() {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<Login />} />
        <Route path="/signup" element={<Signup />} />
        <Route path="/home" element={<Home />} />
      </Routes>
    </BrowserRouter>
  );
}
