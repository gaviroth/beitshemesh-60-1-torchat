import { ipcMain } from "electron";
import { Message, sendMessageToServer } from "./communicator";
import ServerCodes from "./serverCodes";

var handlersUp = false;

export function setupServerEvents() {
  if (handlersUp) return;
  //sign up
  ipcMain.on("signUp", (event, signUpObj) => {
    const message: Message = {
      code: ServerCodes.CLIENT_SIGN_UP,
      data: signUpObj,
    };
    sendMessageToServer(message);
  });
  //login
  ipcMain.on("login", (event, loginObj) => {
    const message: Message = {
      code: ServerCodes.CLIENT_LOG_IN,
      data: loginObj,
    };
    sendMessageToServer(message);
  });
  //send message to the server
  ipcMain.on("sendMessage", (event, messageObj) => {
    const message: Message = {
      code: ServerCodes.MSG_FROM_CLIENT,
      data: messageObj,
    };
    sendMessageToServer(message);
  });
  //block user
  ipcMain.on("blockUser", (event, blockObj) => {
    const message: Message = {
      code: ServerCodes.BLOCK_USER,
      data: blockObj,
    };
    sendMessageToServer(message);
  });

  //logout
  ipcMain.on("logout", (event, logoutObj) => {
    const message: Message = {
      code: ServerCodes.CLIENT_LOG_OUT,
      data: logoutObj,
    };
    sendMessageToServer(message);
  });

  //unblock user
  ipcMain.on("unblockUser", (event, unblockObj) => {
    const message: Message = {
      code: ServerCodes.UNBLOCK_USER,
      data: unblockObj,
    };
    sendMessageToServer(message);
  });

  handlersUp = true;
}
