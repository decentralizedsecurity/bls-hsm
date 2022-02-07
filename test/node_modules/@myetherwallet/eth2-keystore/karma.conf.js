const webpackConfig = require('./webpack.config');

module.exports = function (config) {
    config.set({
        basePath: '',
        frameworks: ['mocha', 'chai'],
        files: ['test/*.spec.js'],
        exclude: [],
        preprocessors: {
            'test/**/*.js': ['webpack']
        },
        webpack: {
            mode: 'production',
            module: webpackConfig.module,
            resolve: webpackConfig.resolve,
            node: webpackConfig.node
        },
        reporters: ['spec'],
        browsers: ['ChromeHeadless'],
        singleRun: true
    });
};
