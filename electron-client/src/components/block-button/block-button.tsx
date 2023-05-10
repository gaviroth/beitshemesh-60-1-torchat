import {Button} from "react-bootstrap";
import css from "./block-button.module.css";

export default function  BlockButton(props:any) {
    return <Button {...props} className={css.block}/>
}