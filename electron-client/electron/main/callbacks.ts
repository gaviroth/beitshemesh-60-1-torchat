import { ipcMain, BrowserWindow } from "electron";
import { registerCallback, Message, sendMessageToServer } from "./communicator";
import ServerCodes from "./serverCodes";

export default function setupServerCallbacks() {
  const win = BrowserWindow.getFocusedWindow();
  //sign up and log in
  registerCallback((message: Message) => {
    if (
      message.code === ServerCodes.CLIENT_SIGNED_UP_SUCCESSFULLY ||
      message.code === ServerCodes.CLIENT_LOGED_IN_SUCCESSFULLY
    ) {
      win?.webContents.send("loginReact", { token: message.data.token });
      console.log(message.data);
    }
  });

  //handle errors
  registerCallback((message: Message) => {
    if (
      [
        ServerCodes.EMAIL_NOT_VALID,
        ServerCodes.PASSWORD_DOSNT_MATCH,
        ServerCodes.TOKEN_ISNT_VALID,
        ServerCodes.PASSWORD_NOT_VALID,
        ServerCodes.USERNAME_ALREADY_TAKEN,
        ServerCodes.USER_DOSNT_EXIST,
      ].includes(message.code)
    ) {
      console.log(message, win);
      win?.webContents.send("serverError", { error: message.data.msg });
    }
  });

  //message recived
  registerCallback((message: Message) => {
    if (message.code === ServerCodes.MSG_TO_CLIENT) {
      win?.webContents.send("newMessage", {
        from: message.data.sendingUser,
        content: message.data.msg,
        id: message.data.msgid.toString(),
      });
      sendMessageToServer({
        code: ServerCodes.MSG_ACK,
        data: { msgid: message.data.msgid.toString() },
      });
    }
  });

}
