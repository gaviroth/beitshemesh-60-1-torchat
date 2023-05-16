import * as net from "net";
import { decrypt, encrypt, getPublicKeyAndN } from "./encrypt";
import ServerCodes from "./serverCodes";

const host = "127.0.0.1";
const ROUTER_PORT = 2345;
const ROUTER_REQUEST = "c";
export const CLIENT_PORT = 4532;


export type Message = {
  code: ServerCodes;
  data: any;
};

const callbacks: ((message: Message) => void)[] = [];

export function registerCallback(callback: (message: Message) => void) {
  callbacks.push(callback);
}

export function litsen() {
  const server = net.createServer((socket) => {
    let recivedMessage = "";
    socket.on("data", (data) => {
      recivedMessage += data;
      
      if (recivedMessage.length < 1) return;
      const code = recivedMessage.charCodeAt(0);
      if (recivedMessage.length < 7) return;
      const length = parseInt(recivedMessage.slice(1, 7));
      if (recivedMessage.length < 7 + length) return;
      const message = recivedMessage.slice(7, 7 + length);
      decrypt(message).then((decryptedMessage) => {
        const data = JSON.parse(decryptedMessage);
        console.log(callbacks.length)
        callbacks.forEach((callback) => {
          console.log(data)
          callback({
            code,
            data,
          });
        });
        recivedMessage = "";
      });
    });
  });
  server.listen(CLIENT_PORT, () => {
    console.log(`Server listening on port ${CLIENT_PORT}`);
  });
}

export async function sendMessageToServer(message: Message): Promise<Message> {
  return new Promise(async (resolve, reject) => {
    const route: MiddleManServer[] = await getRoute();
    const client = new net.Socket();
    const port = Number(route.at(-1)!.port);
    client.connect(port, host, () => {

      // Convert the message data to JSON and encrypt it
      const messageJson = JSON.stringify(message.data);

      const [pkNumber, nNumber] = getPublicKeyAndN();
      const [pk, n] = [
        pkNumber.toString().padStart(2, "0"),
        nNumber.toString().padStart(6, "0"),
      ];
      const needPK = [
        ServerCodes.CLIENT_SIGN_UP,
        ServerCodes.CLIENT_LOG_IN,
      ].includes(message.code);
      // const clientPort = client.localPort?.toString();

      const needPKStr = `${CLIENT_PORT}${pk}${n}`;
      const messageToEncrypt = `${messageJson}${needPK ? needPKStr : ""}`;

      encrypt(messageToEncrypt, route).then((encryptedMessage) => {
        const length = encryptedMessage.length.toString().padStart(6, "0");
        const messageBuffer: string = `${String.fromCharCode(
          message.code
        )}${length}${encryptedMessage}`;

        // Send the message to the server
        client.write(messageBuffer);
      });
    });
  });
}

export type MiddleManServer = {
  port: string;
  publicKey: string;
  n: string;
};

async function getRoute(): Promise<MiddleManServer[]> {
  return sendAndReceiveDataFromSocket(
    ROUTER_REQUEST,
    host,
    ROUTER_PORT,
    48
  ).then((data) => {
    //split the data to 4 parts of 12
    const strRoute = data.match(/.{1,12}/g);
    const route: MiddleManServer[] = strRoute!.map((strSeerver) => {
      return {
        port: strSeerver.slice(0, 4),
        publicKey: strSeerver.slice(4, 6),
        n: strSeerver.slice(6, 12),
      };
    });
    return route;
  });
}

async function sendAndReceiveDataFromSocket(
  character: string,
  host: string,
  port: number,
  bytesToReceive: number
): Promise<string> {
  return new Promise<string>((resolve, reject) => {
    const socket = new net.Socket();

    socket.connect(port, host, () => {
      // Once the socket is connected, send the specified character
      socket.write(character);
    });

    let receivedData = "";

    socket.on("data", (data: Buffer) => {
      // Append the received data to the buffer
      receivedData += data.toString();

      // Check if we have received the desired number of bytes
      if (receivedData.length >= bytesToReceive) {
        // If we have received enough data, close the socket and resolve the promise with the received data
        socket.end();
        resolve(receivedData.slice(0, bytesToReceive));
      }
    });

    socket.on("error", (err: Error) => {
      // If an error occurs, reject the promise with the error message
      reject(err.message);
    });

    socket.on("close", () => {
      // If the socket is closed before we have received the desired number of bytes, reject the promise with an error message
      reject("Socket closed before receiving all expected data");
    });
  });
}