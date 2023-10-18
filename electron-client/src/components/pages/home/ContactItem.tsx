import { ipcRenderer } from "electron";
import { ButtonGroup, ListGroupItem } from "react-bootstrap";

type ContactItempProps = {
  name: string;
  onClick: (contactName: string) => void;
  remove: (contactName: string) => void;
};

export default function ContactItem({
  name,
  onClick,
  remove,
}: ContactItempProps) {
  const blockUser = () => {
    const userData = {
      username: localStorage.getItem("username"),
      blockedUser: name,
      token: localStorage.getItem("token"),
    };
    ipcRenderer.send("blockUser", userData);
  };

  const removeUser = () => {
    remove(name);
  };

  return (
    <ListGroupItem>
      <h2>{name}</h2>
      <ButtonGroup>
        <button className="btn btn-success" onClick={() => onClick(name)}>
          Open
        </button>
        <button className="btn btn-danger" onClick={blockUser}>
          Block
        </button>
        <button className="btn btn-info" onClick={removeUser}>
          Remove
        </button>
      </ButtonGroup>
    </ListGroupItem>
  );
}
