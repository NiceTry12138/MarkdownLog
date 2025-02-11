import "../css/index.css"
// import "style-loader!css-loader!../css/index.css"

import "../css/component.less"
import "../css/prefix.css"

function createElement()
{
    const element = document.createElement('div');
 
    // lodash 现在使用 import 引入。
    element.innerHTML = ['Hello', 'webpack'].join(" ");
    element.classList.add('content');
 
    return element;
}

setTimeout(() => {
    document.body.appendChild(createElement());
}, 100);