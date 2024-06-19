<?php

/**
 * The base configuration for WordPress
 *
 * The wp-config.php creation script uses this file during the installation.
 * You don't have to use the website, you can copy this file to "wp-config.php"
 * and fill in the values.
 *
 * This file contains the following configurations:
 *
 * * Database settings
 * * Secret keys
 * * Database table prefix
 * * ABSPATH
 *
 * @link https://wordpress.org/documentation/article/editing-wp-config-php/
 *
 * @package WordPress
 */

// ** Database settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define('DB_NAME', 'wordpress');

/** Database username */
define('DB_USER', 'root');

/** Database password */
define('DB_PASSWORD', 'password');

/** Database hostname */
define('DB_HOST', 'dev-mysql');

/** Database charset to use in creating database tables. */
define('DB_CHARSET', 'utf8mb4');

/** The database collate type. Don't change this if in doubt. */
define('DB_COLLATE', '');

/**#@+
 * Authentication unique keys and salts.
 *
 * Change these to different unique phrases! You can generate these using
 * the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}.
 *
 * You can change these at any point in time to invalidate all existing cookies.
 * This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
define('AUTH_KEY',         '&k?Z1aZ%d_:qZq2D@i 9.x#/rN[;}~v%#r08}(kTIF5?QX?C*]#fJh,-Db^TlW^H');
define('SECURE_AUTH_KEY',  '_QQ)&lblwmg|&.ZZ[aU|.,e&[|$}9%d@}3qTVmI^*D18/_6O,Jwz:.IkmwY$O#Zs');
define('LOGGED_IN_KEY',    'dpnioRC`hfZjNowx{WdX3V${_G!, :u8S@y|P|M~$c@]tcHrAhuZ)6Z$523cTsIi');
define('NONCE_KEY',        'H:*qq/{N+m1/Boo:pP.v=0iDiU(j}-&4_!Y^qp2-1 L~6(V:Jh2Pr{<UIBp/Nx^8');
define('AUTH_SALT',        '%Pu5xN6-:c;)&4=N+]GkqnAua14m!XZ/M]y1q]4N_fgq*q~0W9![(+|mZLomwI{[');
define('SECURE_AUTH_SALT', 'Gf6Y;8|Xh+mfjcR.$sMQ9IHv|~p&qb72Y^}+J)uz~]~cMuyZawjk4OPmCR^?}*>{');
define('LOGGED_IN_SALT',   '`-]j:urqjQ)qhEfVrTsazT2KA}P|648<dS+:`?(</<ptASL2{2$wkuti;Wf`WPF#');
define('NONCE_SALT',       'e=6USZC/KA0MaUn:3qvsa$]SnnTBLMaa-*=|Ff4bt+ETfUY(ZXSCde$}X-&SvM,R');

/**#@-*/

/**
 * WordPress database table prefix.
 *
 * You can have multiple installations in one database if you give each
 * a unique prefix. Only numbers, letters, and underscores please!
 */
$table_prefix = 'wp_';

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 *
 * For information on other constants that can be used for debugging,
 * visit the documentation.
 *
 * @link https://wordpress.org/documentation/article/debugging-in-wordpress/
 */
define('WP_DEBUG', true);

/* Add any custom values between this line and the "stop editing" line. */

require_once __DIR__ . '/vendor/autoload.php';

class WordPressDefaultRequestPlugin extends Pinpoint\Plugins\DefaultRequestPlugin
{
    public function __construct()
    {
        $blackUri = ['/favicon.ico'];
        // if uri in blackUri, skips it 
        if (!in_array($_SERVER['REQUEST_URI'], $blackUri)) {
            parent::__construct();
        }
    }
    public function __destruct()
    {
        // do nothing
    }
}
define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.wordpress');  // your application id
define('PP_REQ_PLUGINS', WordPressDefaultRequestPlugin::class);
require_once __DIR__ . '/vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';

/* That's all, stop editing! Happy publishing. */

/** Absolute path to the WordPress directory. */
if (!defined('ABSPATH')) {
    define('ABSPATH', __DIR__ . '/');
}

/** Sets up WordPress vars and included files. */
require_once ABSPATH . 'wp-settings.php';
