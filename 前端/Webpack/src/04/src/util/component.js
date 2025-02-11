import "../css/index.css"
// import "style-loader!css-loader!../css/index.css"

import "../css/component.less"
import "../css/prefix.css"

import miku from "../img/miku.jpg"

function createElement()
{
    const element = document.createElement('div');
 
    // lodash 现在使用 import 引入。
    element.innerHTML = ['Hello', 'webpack'].join(" ");
    element.classList.add('content');

    let img = new Image();
    img.src = require("../img/miku.jpg").default;
    element.appendChild(img);
 
    let img2 = new Image();
    img2.src = miku;
    element.appendChild(img2);
 
    return element;
}

setTimeout(() => {
    document.body.appendChild(createElement());
}, 100);