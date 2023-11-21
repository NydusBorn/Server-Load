package com.nydus.example.server_load_android

import android.content.SharedPreferences
import androidx.compose.runtime.mutableFloatStateOf
import androidx.navigation.NavController
import java.net.Socket

object Connector{
    var BottomNavBarVisibility = mutableFloatStateOf(0f)
    var NavController: NavController? = null
    var Connection: Socket? = null
    var AppPreferences: SharedPreferences? = null
}
