const { options } = require("less")
const { type } = require("os")
const path = require("path")
const loader = require("sass-loader")

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
                    { 
                        loader: 'css-loader',
                        options: {
                            importLoaders: 1,
                            esModule: false,
                        }
                    },
                    {
                        loader: "postcss-loader",
                        options: {
                            postcssOptions: {
                                plugins: [
                                    require("postcss-preset-env"),
                                ]
                            }
                        }
                    }
                ],
            },
            {
                test: /\.less$/,
                use: [
                    "style-loader",
                    "css-loader",
                    "less-loader"
                ]
            },
            // {
            //     test: /\.(png|jpg|jpeg|gif|svg)$/,
            //     type: "asset/resource",
            //     generator: {
            //         publicPath: 'assets/',
            //         filename: 'img/[name]-[hash][ext]',
            //     },
            // }, 
            // {
            //     test: /\.(png|jpg|jpeg|gif|svg)$/,
            //     type: "asset/inline",
            // }
            {
                test: /\.(png|jpg|jpeg|gif|svg)$/,
                type: "asset",
                generator: {
                    filename: "image/[name]-[hash:6][ext]"
                },
                parser: {
                    dataUrlCondition: {
                        maxSize: 10 * 1024 // 10kb
                    }
                }
            }
        ],
    },
}