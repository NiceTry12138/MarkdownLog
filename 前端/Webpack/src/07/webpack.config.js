const path = require("path")

const { CleanWebpackPlugin } = require('clean-webpack-plugin')
const HtmlWebpackPlugin = require(`html-webpack-plugin`)
const { DefinePlugin } = require('webpack')

module.exports = {
    mode: "development",
    // mode: "production",
    entry: "./src/main.js",
    devtool: "source-map",
    output: {
        filename: "./bundle.js",
        path: path.resolve(__dirname, "./build")
    },
    plugins: [
        new CleanWebpackPlugin(),
        new HtmlWebpackPlugin({
            template: "./index.html"
        }),
        new DefinePlugin({
            BASE_URL: '"./"'
        }),
    ]
}