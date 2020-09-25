var webpack = require('webpack');
module.exports = {
	context: __dirname + '/js',
	entry: {
  	app: './app.js',
  	vendor: ['angular']
	},
 	output: {
  	path: __dirname + '/bundles',
  	filename: 'app.bundle.js'
 	},
 	plugins: [
  	new webpack.optimize.CommonsChunkPlugin("vendor", "vendor.bundle.js")
 	]
};