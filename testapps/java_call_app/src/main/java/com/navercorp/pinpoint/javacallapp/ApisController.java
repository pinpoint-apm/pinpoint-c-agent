package com.navercorp.pinpoint.javacallapp;

import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.apache.commons.io.IOUtils;


/**
 * Created by chenguoxi on 8/24/17.
 */
@Controller
public class ApisController {
    @RequestMapping(value = { "/index.html", "/apis" }, method = RequestMethod.GET)
    public String apis(Model model) {
        return "apis";
    }

    @RequestMapping(value = {"/call" }, method = RequestMethod.GET)
    @ResponseBody
    public String calls(@RequestParam("address") String address) {
        String callString = "call: address=" + address;

        CloseableHttpClient httpclient = HttpClients.createDefault();

        HttpGet httpGet = new HttpGet(address);
        try {
            CloseableHttpResponse response1 = httpclient.execute(httpGet);

            try {
                return IOUtils.toString(response1.getEntity().getContent());
            } finally {
                try {
                    response1.close();
                } catch (Exception e) {
                    callString = callString + "; exception=" + e.toString();
                }
            }

        } catch (Exception e) {
            callString = callString + "; exception=" + e.toString();
        } finally {
            try {
                httpclient.close();
            } catch (Exception e) {
                callString = callString + "; exception=" + e.toString();
            }
        }

        return callString;

    }
}
