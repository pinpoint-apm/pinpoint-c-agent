## Configuration

Name|Use|Optional|Example
---|----|----|----
PP_REPORT_MEMORY_USAGE|report memory usage by memory_get_usage\|memory_get_peak_usage|Yes|define('PP_REPORT_MEMORY_USAGE','1'); ## enable 
PINPOINT_USE_CACHE| Use the cached class (within `AOP_CACHE_DIR`)|Yes| define('PINPOINT_USE_CACHE','YES'); <br> strongly recommend `Yes` for real env.
PP_REQ_PLUGINS|Per request class name|Yes | define('PP_REQ_PLUGINS', '\Plugins\PerRequestPlugins');
