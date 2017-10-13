package cxw.cn.svplayer.example.common;

import android.os.Environment;
import android.text.TextUtils;
import android.util.Log;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by cxw on 2017/10/2.
 */

public class LogUtils {
    /**
     * ALL
     */
    public static final int LOG_ALL_TO_FILE = 3;
    /**
     * ERROR
     */
    public static final int LOG_ERROR_TO_FILE = 2;
    /**
     * None
     */
    public static final int LOG_NONE_TO_FILE = 0;
    /**
     * WARN
     */
    public static final int LOG_WARN_TO_FILE = 1;
    /**
     * INFO
     */
    public static final int LOG_INFO_TO_FILE = 4;

    /**
     * The TAG of the Application
     */
    public static String TAG = "svplayer_example";
    /**
     * Whether to enable the log
     */
    protected static boolean isEnable = true;
    /**
     * The log dir path
     */
    protected static String logDirPath = Environment.getExternalStorageDirectory() + "/svplayer_example";
    /**
     * The log file base name
     */
    protected static String logFileBaseName = "/log";
    /**
     * The log file suffix,such as log.
     */
    protected static String logFileSuffix = "log";
    /**
     * The log file path
     */
    private static String path = "";
    /**
     * Which will be logged into the file
     */
    protected static int policy = LOG_NONE_TO_FILE;

    private static ExecutorService executor = null;

    private static long logFileSize = 10*1024*1024;

    /**
     * the constructor
     */
    private LogUtils() {
    }

    enum TYPE {
        INFO, DEBUG, VERBOSE, WARN, ERROR
    }

    /**
     * Send a DEBUG log message.
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void d(String tag, String msg) {
        if (isEnable) {
            if (tag == null || tag == "") {
                d(msg);
            } else {
                android.util.Log.d(tag, buildMessage(TYPE.DEBUG, tag, msg, null));
            }
        }
    }

    /**
     * Send a DEBUG log message.
     */
    public static void d(String msg) {
        if (isEnable) {
            android.util.Log.d(TAG, buildMessage(TYPE.DEBUG, TAG, msg, null));
        }
    }

    /**
     * Building Message
     *
     * @param msg
     *            The message you would like logged.
     * @return Message String
     */
    protected static String buildMessage(TYPE type, String tag, String msg, Throwable thr) {

        StackTraceElement caller = new Throwable().fillInStackTrace().getStackTrace()[2];

        boolean isLog2File = false;

        switch (policy) {
            case LOG_NONE_TO_FILE:
                isLog2File = false;
                break;
            case LOG_WARN_TO_FILE:
                if (type == TYPE.WARN) {
                    isLog2File = true;
                } else {
                    isLog2File = false;
                }
                break;
            case LOG_ERROR_TO_FILE:
                if (type == TYPE.ERROR) {
                    isLog2File = true;
                } else {
                    isLog2File = false;
                }
                break;
            case LOG_ALL_TO_FILE:
                isLog2File = true;
                break;
            case LOG_INFO_TO_FILE:
                if (type == TYPE.INFO) {
                    isLog2File = true;
                } else {
                    isLog2File = false;
                }
                break;
            default:
                break;
        }

        // The log will be shown in logcat.
        StringBuffer bufferlog = new StringBuffer();
        //bufferlog.append(caller.getClassName());
        //bufferlog.append(".");
        bufferlog.append(caller.getMethodName());
        bufferlog.append("( ");
        bufferlog.append(caller.getFileName());
        bufferlog.append(": ");
        bufferlog.append(caller.getLineNumber());
        bufferlog.append(")");
        bufferlog.append(" : ");
        bufferlog.append(msg);
        if (thr != null) {
            bufferlog.append(System.getProperty("line.separator"));
            bufferlog.append(android.util.Log.getStackTraceString(thr));
        }

        // set the default log path
        if (TextUtils.isEmpty(path)) {
            //setPath(logDirPath, logFileBaseName, logFileSuffix);
            isLog2File = false;
        }

        if (isLog2File) {
            // The log will be written in the log file.
            StringBuffer filelog = new StringBuffer();

            Date myDate = new Date();
            SimpleDateFormat fdf = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");
            String myDateString = fdf.format(myDate);

            filelog.append(myDateString);
            filelog.append("    ");
            filelog.append(type.name().charAt(0));
            filelog.append("    ");
            filelog.append(tag);
            filelog.append("    ");
            filelog.append(bufferlog);

            log2file(path, filelog.toString());
        }

        return bufferlog.toString();
    }

    /**
     * Send a DEBUG log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void d(String tag, String msg, Throwable thr) {
        if (isEnable) {
            if (tag == null || tag == "") {
                d(msg, thr);
            } else {
                android.util.Log.d(tag, buildMessage(TYPE.DEBUG, tag, msg, thr), thr);
            }
        }
    }

    /**
     * Send a DEBUG log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void d(String msg, Throwable thr) {
        if (isEnable) {
            android.util.Log.d(TAG, buildMessage(TYPE.DEBUG, TAG, msg, thr), thr);
        }
    }

    /**
     * Send a ERROR log message.
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void e(String tag, String msg) {
        if (isEnable) {
            if (tag == null || tag == "") {
                e(msg);
            } else {
                android.util.Log.e(tag, buildMessage(TYPE.ERROR, tag, msg, null));
            }
        }
    }

    /**
     * Send an ERROR log message.
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void e(String msg) {
        if (isEnable) {
            android.util.Log.e(TAG, buildMessage(TYPE.ERROR, TAG, msg, null));
        }
    }

    /**
     * Send a ERROR log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void e(String tag, String msg, Throwable thr) {
        if (isEnable) {
            if (tag == null || tag == "") {
                e(msg, thr);
            } else {
                android.util.Log.e(tag, buildMessage(TYPE.ERROR, tag, msg, thr), thr);
            }
        }
    }

    /**
     * Send an ERROR log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void e(String msg, Throwable thr) {
        if (isEnable) {
            android.util.Log.e(TAG, buildMessage(TYPE.ERROR, TAG, msg, thr), thr);
        }
    }

    /**
     * Send a VERBOSE log message.
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void v(String tag, String msg) {
        if (isEnable) {
            if (tag == null || tag == "") {
                v(msg);
            } else {
                android.util.Log.v(tag, buildMessage(TYPE.VERBOSE, tag, msg, null));
            }
        }
    }

    /**
     * Send a VERBOSE log message.
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void v(String msg) {
        if (isEnable) {
            android.util.Log.v(TAG, buildMessage(TYPE.VERBOSE, TAG, msg, null));
        }
    }

    /**
     * Send a VERBOSE log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void v(String tag, String msg, Throwable thr) {
        if (isEnable) {
            if (tag == null || tag == "") {
                v(msg, thr);
            } else {
                android.util.Log.v(tag, buildMessage(TYPE.VERBOSE, tag, msg, thr), thr);
            }
        }
    }

    /**
     * Send a VERBOSE log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void v(String msg, Throwable thr) {
        if (isEnable) {
            android.util.Log.v(TAG, buildMessage(TYPE.VERBOSE, TAG, msg, thr), thr);
        }
    }

    /**
     * Send a INFO log message.
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void i(String tag, String msg) {
        if (isEnable) {
            if (tag == null || tag == "") {
                i(msg);
            } else {
                android.util.Log.i(tag, buildMessage(TYPE.INFO, tag, msg, null));
            }
        }
    }

    /**
     * Send an INFO log message.
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void i(String msg) {
        if (isEnable) {
            android.util.Log.i(TAG, buildMessage(TYPE.INFO, TAG, msg, null));
        }
    }

    /**
     * Send a INFO log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void i(String tag, String msg, Throwable thr) {
        if (isEnable) {
            if (tag == null || tag == "") {
                i(msg, thr);
            } else {
                android.util.Log.i(tag, buildMessage(TYPE.INFO, tag, msg, thr), thr);
            }
        }
    }

    /**
     * Send a INFO log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void i(String msg, Throwable thr) {
        if (isEnable) {
            android.util.Log.i(TAG, buildMessage(TYPE.INFO, TAG, msg, thr), thr);
        }
    }

    /**
     * Send an empty WARN log message and log the exception.
     *
     * @param thr
     *            An exception to log
     */
    public static void w(Throwable thr) {
        if (isEnable) {
            android.util.Log.w(TAG, buildMessage(TYPE.WARN, TAG, "", thr), thr);
        }
    }

    /**
     * Send a WARN log message.
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void w(String tag, String msg) {
        if (isEnable) {
            if (tag == null || tag == "") {
                w(msg);
            } else {
                android.util.Log.w(tag, buildMessage(TYPE.WARN, tag, msg, null));
            }
        }
    }

    /**
     * Send a WARN log message
     *
     * @param msg
     *            The message you would like logged.
     */
    public static void w(String msg) {
        if (isEnable) {
            android.util.Log.w(TAG, buildMessage(TYPE.WARN, TAG, msg, null));
        }
    }

    /**
     * Send a WARN log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void w(String tag, String msg, Throwable thr) {
        if (isEnable) {
            if (tag == null || tag == "") {
                w(msg, thr);
            } else {
                android.util.Log.w(tag, buildMessage(TYPE.WARN, tag, msg, thr), thr);
            }
        }
    }

    /**
     * Send a WARN log message and log the exception.
     *
     * @param msg
     *            The message you would like logged.
     * @param thr
     *            An exception to log
     */
    public static void w(String msg, Throwable thr) {
        if (isEnable) {
            android.util.Log.w(TAG, buildMessage(TYPE.WARN, TAG, msg, thr), thr);
        }
    }

    /**
     * Get the ExecutorService
     *
     * @return the ExecutorService
     */
    public static ExecutorService getExecutor() {
        return getExecutor();
    }

    /**
     * Set the ExecutorService
     *
     * @param executor
     *            the ExecutorService
     */
    public static void setExecutor(ExecutorService executor) {
        setExecutor(executor);
    }

    /**
     * get the log file path
     *
     * @return path
     */
    public static String getPath() {
        return path;
    }

    /**
     * set the path of the log file
     *
     * @param path
     */
    public static void setPath(String path) {
        path = path;
        System.out.println("LogUtils.path = " + path);
        createLogDir(path);
    }

    /**
     * create the Directory from the path
     *
     * @param path
     */
    private static void createLogDir(String path) {
        android.util.Log.e("Path", path);
        if (TextUtils.isEmpty(path)) {
            android.util.Log.e("Error", "The path is not valid.");

            return;
        }

        File file = new File(path);

        boolean ret;
        boolean exist;

        exist = file.getParentFile().exists();
        if (!exist) {
            ret = file.getParentFile().mkdirs();

            if (!ret) {
                android.util.Log.e("Error", "The Log Dir can not be created!");
                return;
            }
            android.util.Log.i("Success", "The Log Dir was successfully created! -" + file.getParent());
        }
    }

    /**
     * get the policy of the log
     *
     * @return the policy of the log
     */
    public static int getPolicy() {
        return policy;
    }

    /**
     * set the policy of the log
     *
     * @param policy
     *            the policy of the log
     */
    public static void setPolicy(int policy) {
        policy = policy;
    }

    /**
     * Handy function to get a loggable stack trace from a Throwable
     *
     * @param tr
     *            An exception to log
     * @return
     */
    public static String getStackTraceString(Throwable tr) {
        return android.util.Log.getStackTraceString(tr);
    }

    /**
     * Get the Tag of the application
     */
    public static String getTag() {
        return TAG;
    }

    /**
     * Set the Tag of the application
     *
     * @param tag
     *            the Tag of the application
     */
    public static void setTag(String tag) {
        TAG = tag;
    }

    /**
     * is the log enabled?
     */
    public static boolean isEnabled() {
        return isEnable;
    }

    /**
     * enable or disable the log
     *
     * @param enabled
     *            whether to enable the log
     */
    public static void setEnabled(boolean enabled) {
        isEnable = enabled;
    }

    /**
     * Checks to see whether or not a log for the specified tag is loggable at
     * the specified level. The default level of any tag is set to INFO. This
     * means mContainer any level above and including INFO will be logged. Before you
     * make any calls to a logging method you should check to see if your tag
     * should be logged. You can change the default level by setting a system
     * property: 'setprop log.tag.<YOUR_LOG_TAG> <LEVEL>' Where level is either
     * VERBOSE, DEBUG, INFO, WARN, ERROR, ASSERT, or SUPPRESS. SUPPRESS will
     * turn off all logging for your tag. You can also create a local.prop file
     * mContainer with the following in it: 'log.tag.<YOUR_LOG_TAG>=<LEVEL>' and place
     * mContainer in /data/local.prop.
     *
     * @param tag
     *            The tag to check
     * @param level
     *            The level to check
     * @return Whether or not mContainer this is allowed to be logged.
     */
    public static boolean isLoggable(String tag, int level) {
        return android.util.Log.isLoggable(tag, level);
    }

    /**
     * Low-level logging call.
     *
     * @param priority
     *            The priority/type of this log message
     * @param tag
     *            Used to identify the source of a log message. It usually
     *            identifies the class or activity where the log call occurs.
     * @param msg
     *            The message you would like logged.
     * @return The number of bytes written.
     */
    public static int println(int priority, String tag, String msg) {
        return android.util.Log.println(priority, tag, msg);
    }

    /**
     * set the log file path
     * <p/>
     * The log file path will be: logDirPath + logFileBaseName + Formated time
     * +logFileSuffix
     *
     * @param logDirPath
     *            the log file dir path,such as "/mnt/sdcard/snowdream/log"
     * @param logFileBaseName
     *            the log file base file name,such as "log"
     * @param logFileSuffix
     *            the log file suffix,such as "log"
     */
    public static void setPath(String logDirPath, String logFileBaseName, String logFileSuffix) {
        if (!TextUtils.isEmpty(logDirPath)) {
            logDirPath = logDirPath;
        }

        if (!TextUtils.isEmpty(logFileBaseName)) {
            logFileBaseName = logFileBaseName;
        }

        if (!TextUtils.isEmpty(logFileSuffix)) {
            logFileSuffix = logFileSuffix;
        }

        Date myDate = new Date();
        SimpleDateFormat fdf = new SimpleDateFormat("yyyy-MM-dd");
        String myDateString = fdf.format(myDate);

        StringBuffer buffer = new StringBuffer();
        buffer.append(logDirPath);
        if (!logDirPath.endsWith("/")) {
            buffer.append("/");
        }
//        buffer.append(logFileBaseName);
//        buffer.append("-");
        buffer.append(myDateString);
        buffer.append(".");
        buffer.append(logFileSuffix);

        cleanFiles(myDateString);
        setPath(buffer.toString());
    }

    /**
     * 清除不是当前的文件。防止日志文件不停的增多。
     * @param myDateString
     */
    private static void cleanFiles(String todayFileName) {
        if (!TextUtils.isEmpty(logDirPath)){
            File dir = new File(logDirPath);
            File[] files = dir.listFiles();
            if (files == null){
                return;
            }
            for (int i = 0; i < files.length; i++) {
                if (files[i] != null && !todayFileName.equals(files[i].getName())){
                    files[i].delete();
                }
            }
        }
    }

    protected static void log2file(final String path, final String str) {
        if (executor == null) {
            executor = Executors.newSingleThreadExecutor();
        }

        if (executor != null) {
            executor.execute(new Runnable() {
                @Override
                public void run() {
                    PrintWriter out = null;

                    File file = GetFileFromPath(path);

                    try {
                        out = new PrintWriter(new BufferedWriter(new FileWriter(file, true)));
                        out.println(str);
                        out.flush();
                    } catch (IOException e) {
                        e.printStackTrace();
                    } finally {
                        if(out != null){
                            out.close();
                        }
                    }
                }
            });
        }
    }

    /**
     * Get File form the file path.<BR>
     * if the file does not exist, create it and return it.
     *
     * @param path
     *            the file path
     * @return the file
     */
    private static File GetFileFromPath(String path) {
        boolean ret;
        boolean isExist;
        boolean isWritable;
        File file = null;

        if (TextUtils.isEmpty(path)) {
            Log.e("Error", "The path of Log file is Null.");
            return file;
        }

        file = new File(path);

        isExist = file.exists();
        isWritable = file.canWrite();

        if (isExist) {
            if(file.length() > logFileSize){
                boolean isDelete = file.delete();
                if(isDelete){
                    return GetFileFromPath(path);
                }else{
                    Log.e(TAG, "delete logfile failed");
                }
            }
            if (isWritable) {
                // Log.i("Success", "The Log file exist,and can be written! -" +
                // file.getAbsolutePath());
            } else {
                Log.e("Error", "The Log file can not be written.");
            }
        } else {
            // create the log file
            try {
                ret = file.createNewFile();
                if (ret) {
                    Log.i("Success", "The Log file was successfully created! -" + file.getAbsolutePath());
                } else {
                    Log.i("Success", "The Log file exist! -" + file.getAbsolutePath());
                }

                isWritable = file.canWrite();
                if (!isWritable) {
                    Log.e("Error", "The Log file can not be written.");
                }
            } catch (IOException e) {
                Log.e("Error", "Failed to create The Log file.");
                e.printStackTrace();
            }
        }

        return file;
    }

}
