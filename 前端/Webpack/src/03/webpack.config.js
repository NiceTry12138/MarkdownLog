const path = require("path")

module.exports = {
    entry: "./src/index.js",
    output: {
        filename: "./bundle.js",
        path: path.resolve(__dirname, "./build")
    },    
    module: {
        rules: [
            {
                test: /\.css$/,
                use: [
                    { loader: 'style-loader' },
                    { loader: 'css-loader' },
                    // { loader: 'sass-loader' },
                ],
            },
        ],
    },
}