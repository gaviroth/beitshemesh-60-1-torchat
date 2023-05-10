import { ipcRenderer } from "electron";
import { useEffect, useState } from "react";
import {
  Container,
  Row,
  Col,
  Form,
  InputGroup,
  Button,
  ListGroup,
  Alert,
} from "react-bootstrap";
import ContactItem from "./ContactItem";
import Conversation from "./Conversation";
import BlockButton from "@/components/block-button/block-button";
import { useNavigate } from "react-router-dom";

export type Contact = {
  name: string;
  converstation: ContactMessage[];
};

export type ContactMessage = {
  from: string;
  to: string;
  message: string;
  id: string;
};

function loadChats(): Contact[] {
  const storageData = localStorage.getItem(
    `chats#${localStorage.getItem("username")}`
  );
  if (!storageData) {
    localStorage.setItem(
      `chats#${localStorage.getItem("username")}`,
      JSON.stringify([])
    );
    return [];
  }
  return JSON.parse(storageData);
}

function loadContact(name: string): Contact | undefined {
  const contacts = loadChats();
  const contact = contacts.find((contact) => contact.name === name);
  if (contact) {
    return contact;
  }
  contacts.push({ name: name, converstation: [] });
  localStorage.setItem(
    `chats#${localStorage.getItem("username")}`,
    JSON.stringify(contacts)
  );
  return loadContact(name);
}

function saveMessage(name: string, message: ContactMessage) {
  const contacts = loadChats();
  let contact = contacts.find((contact) => contact.name === name);
  if (!contact) {
    const newContact = { name, converstation: [] };
    contacts.push(newContact);
    localStorage.setItem(
      `chats#${localStorage.getItem("username")}`,
      JSON.stringify(contacts)
    );
    contact = newContact;
  }
  if (contact.converstation.some((msg) => msg.id === message.id)) {
    return;
  }
  contact.converstation.push(message);
  localStorage.setItem(
    `chats#${localStorage.getItem("username")}`,
    JSON.stringify(contacts)
  );
}

export default function Home() {
  const navigate = useNavigate();
  const [message, setMessage] = useState<string>("");
  const [alert, setAlert] = useState("");
  const [chat, setChat] = useState<ContactMessage[]>([]);
  const [myContacts, setMyContacts] = useState<Contact[]>([]);
  const [currentContact, setCurrentContact] = useState<Contact | undefined>(
    undefined
  );
  const [contactSearch, setContactSearch] = useState<string>("");

  useEffect(() => {
    getContacts();
    ipcRenderer.on("newMessage", (event, arg: any) => {
      console.log("message reciaved");
      //handle message recived here
      const message: ContactMessage = {
        from: arg.from,
        to: localStorage.getItem("username")!,
        id: arg.id,
        message: arg.content,
      };
      saveMessage(message.from, message);
      if (currentContact && currentContact.name === message.from) {
        openContact(currentContact.name);
      }
    });

    ipcRenderer.on("serverError", (event, arg) => {
      setAlert(arg.error);
    });
    // return () => {
    //   ipcRenderer.removeAllListeners("newMessage");
    //   ipcRenderer.removeAllListeners("serverError");
    // };
  }, []);

  useEffect(() => {
    const chatToSet = currentContact?.converstation ?? [];
    setChat([...chatToSet]);
  }, [currentContact]);

  const logout = () => {
    const logoutData = {
      username: localStorage.getItem("username"),
    };
    ipcRenderer.send("logout", logoutData);
    navigate("/");
  };

  const sendMessage = () => {
    if (!currentContact) {
      return;
    }
    const messageData = {
      sendingUser: localStorage.getItem("username"),
      receivingUser: currentContact.name,
      token: localStorage.getItem("token"),
      msg: message,
    };
    setMessage("");
    ipcRenderer.send("sendMessage", messageData);
    saveMessage(currentContact.name, {
      from: localStorage.getItem("username")!,
      to: currentContact.name,
      id: window.crypto.randomUUID(),
      message: message,
    });
    setCurrentContact(loadContact(currentContact.name));
  };

  const openContact = (name: string): void => {
    setCurrentContact(loadContact(name));
    getContacts();
  };

  const removeContact = (name: string): void => {
    if (currentContact && currentContact.name === name) {
      setCurrentContact(undefined);
    }
    const newContacts = myContacts.filter((contact) => contact.name !== name);
    console.log(newContacts, getContacts(), JSON.stringify(newContacts));
    localStorage.setItem(
      `chats#${localStorage.getItem("username")}`,
      JSON.stringify(newContacts)
    );
    getContacts();
  };

  const getContacts = () => {
    setMyContacts(loadChats());
  };

  const contacts = myContacts.map((contact: Contact) => {
    return (
      <ContactItem
        name={contact.name}
        key={contact.name}
        onClick={openContact}
        remove={removeContact}
      />
    );
  });

  const alertDOM = (
    <Row>
      <Col xs={12}>
        <Alert variant="danger" onClose={() => setAlert("")} dismissible>
          {alert}
        </Alert>
      </Col>
    </Row>
  );

  const openContactHeader = (
    <Row>
      <Col xs={12}>
        <h1>{currentContact?.name}</h1>
      </Col>
    </Row>
  );

  return (
    <Container>
      <Row>
        <Col xs={4}>
          <Container>
            <Row>
              <Col xs={12}>
                <BlockButton variant="danger" onClick={logout}>
                  Logout
                </BlockButton>
              </Col>
            </Row>
            <Row>
              <Col xs={12}>
                <InputGroup>
                  <Form.Control
                    type="text"
                    placeholder="Enter contact data here"
                    onChange={(e) => setContactSearch(e.target.value)}
                  />
                  <Button
                    variant="primary"
                    onClick={() => openContact(contactSearch)}
                  >
                    Add
                  </Button>
                </InputGroup>
              </Col>
            </Row>
            <Row>
              <Col xs="12">
                <ListGroup>{contacts}</ListGroup>
              </Col>
            </Row>
          </Container>
        </Col>
        <Col xs={8}>
          <Container>
            {alert && alertDOM}
            {currentContact && openContactHeader}
            <Row>
              <Col xs={12}>
                {currentContact && (
                  <Conversation
                    contactName={currentContact.name}
                    messages={chat}
                  />
                )}
              </Col>
            </Row>
            <Row>
              <Col xs={12}>
                <InputGroup>
                  <Form.Control
                    value={message}
                    type="text"
                    placeholder="Enter your message here"
                    onChange={(e) => setMessage(e.target.value)}
                  />
                  <Button
                    variant="primary"
                    onClick={sendMessage}
                    disabled={currentContact == null}
                  >
                    Send
                  </Button>
                </InputGroup>
              </Col>
            </Row>
          </Container>
        </Col>
      </Row>
    </Container>
  );
}
