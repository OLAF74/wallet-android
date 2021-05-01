package ru.olaf.custom;


import android.util.Log;

public class AddressDecoder {

    private static final String TAG = "AddressDecoder";

    static {
        System.loadLibrary("address.converter");
        Log.d(TAG, "static initializer: lib address.converter loaded");
    }

    public static native String convertToHex(String address);

    public static native String convertToBase64(String address, boolean bounceable, boolean test, boolean as_url);
}