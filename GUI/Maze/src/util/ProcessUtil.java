package util;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.charset.Charset;

public class ProcessUtil {
    /**
     * exePath：exe文件路径
     * stdinText：exe文件接收的内容
     * */
    public static String runAndRead(String exePath,String stdinText) throws Exception {
        ProcessBuilder pb = new ProcessBuilder(exePath);
        pb.redirectErrorStream(true);
        Process p = pb.start();
        if (stdinText != null && !stdinText.isEmpty()) {
            try (BufferedWriter bw = new BufferedWriter(
                    new OutputStreamWriter(p.getOutputStream(), Charset.defaultCharset()))) {
                bw.write(stdinText);
                bw.flush();
            }
        } else {p.getOutputStream().close();}
        StringBuilder sb = new StringBuilder();
        try (BufferedReader br = new BufferedReader(
                new InputStreamReader(p.getInputStream(), Charset.defaultCharset()))) {
            String line;
            while ((line = br.readLine()) != null)
            {sb.append(line.trim());}
        }
        int code = p.waitFor();
        String out = sb.toString();
        if (code != 0) {
            throw new RuntimeException("Process failed: " + exePath + ", code=" + code + ", out=" + out);
        }
        return out;
    }
}
