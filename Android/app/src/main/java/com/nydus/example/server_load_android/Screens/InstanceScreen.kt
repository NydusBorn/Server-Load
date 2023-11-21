package com.nydus.example.server_load_android.Screens

import android.app.AlertDialog
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.TextField
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.window.Dialog
import com.nydus.example.server_load_android.Connector
import java.lang.Exception
import java.net.InetSocketAddress
import java.net.Socket


@Composable
fun InstanceScreen (){
    if (Connector.AppPreferences?.getString("last_instance", "none") != "none") {
        val lastInstance = Connector.AppPreferences?.getString("last_instance", "none")?.split(":")
        try {
            val socket = Socket()
            socket.soTimeout = 5000
            socket.connect(InetSocketAddress(lastInstance?.get(0), lastInstance?.get(1)?.toInt()!!), 5000)
            Connector.Connection = socket
            return
        }
        catch (e:Exception){
            AlertDialog.Builder(Connector.NavController!!.context).setTitle("Error").setMessage("${e.message.toString()}\n${e.javaClass}\n${e.stackTrace[0]}").show()
            Connector.AppPreferences?.edit()?.putString("last_instance", "none")?.apply()
        }
    }
    Connector.BottomNavBarVisibility.floatValue = 0f
    Column(verticalArrangement = Arrangement.Center, horizontalAlignment = Alignment.CenterHorizontally, modifier = Modifier.fillMaxSize()) {
        TextField(value = "", onValueChange = {}, placeholder = { Text(text = "Instance IP address")})
        TextField(value = "", onValueChange = {}, placeholder = { Text(text = "Instance listen port")})
        Button(onClick = {
            val lastInstance = Connector.AppPreferences?.getString("last_instance", "none")?.split(":")
            try {
                val socket = Socket()
                socket.soTimeout = 5000
                socket.connect(InetSocketAddress(lastInstance?.get(0), lastInstance?.get(1)?.toInt()!!), 5000)
                Connector.Connection = socket
                Connector.NavController?.navigate("Building")
                Connector.BottomNavBarVisibility.floatValue = 1f
            }
            catch (e:Exception){
                AlertDialog.Builder(Connector.NavController!!.context).setTitle("Error").setMessage("${e.message.toString()}\n${e.javaClass}\n${e.stackTrace[0]}").show()
                Connector.AppPreferences?.edit()?.putString("last_instance", "none")?.apply()
            }
            
        }) {
            Text(text = "Enter")
        }
    }
}