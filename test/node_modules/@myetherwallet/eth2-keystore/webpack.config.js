module.exports = {
    entry: './src/index.js',
    mode: 'production',
    node: {
        fs: 'empty'
    },
    output: {
        filename: 'dist/bundle.js'
    },
    resolve: {
        extensions: ['.js']
    },
    module: {
        rules: []
    }
};
