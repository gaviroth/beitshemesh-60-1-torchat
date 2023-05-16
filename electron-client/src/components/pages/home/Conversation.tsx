import { ListGroup, ListGroupItem } from "react-bootstrap";
import { ContactMessage } from "./Home";

type ConversationProps = {
  contactName: string;
  messages: ContactMessage[];
};

export default function Conversation({
  contactName,
  messages,
}: ConversationProps) {
  console.log(messages);
  const messagesDOM = messages.map((message, index) => {
    if (message.from === contactName) {
      return (
        <ListGroupItem key={index} style={{ color: "black" }}>
          <h2>{contactName}:</h2>
          <p>{message.message}</p>
        </ListGroupItem>
      );
    } else {
      return (
        <ListGroupItem key={index} style={{ color: "blue" }}>
          <h2>{localStorage.getItem("username")}:</h2>
          <p>{message.message}</p>
        </ListGroupItem>
      );
    }
  });

  return <ListGroup>{messagesDOM}</ListGroup>;
}
