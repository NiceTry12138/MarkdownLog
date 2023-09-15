const ejs = require('ejs')
const path = require('path')

const compile = (template, data) => {
    const templatePosition = `../template/${template}`
    const templatePath = path.resolve(__dirname, templatePosition)
    // console.log(templatePath)
    return new Promise((resolve, reject) => {
        ejs.renderFile(templatePath, {data}, {}, (err, result) => {
            if(err) {
                reject(err);
                return;
            }

            resolve(result);
        })
    })
}

module.exports = {
    compile
}
