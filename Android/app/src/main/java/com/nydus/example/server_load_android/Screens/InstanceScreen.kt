package com.nydus.example.server_load_android.Screens

import android.app.AlertDialog
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.TextField
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.window.Dialog
import androidx.navigation.NavController
import com.nydus.example.server_load_android.Connector
import com.nydus.example.server_load_android.GameState
import com.nydus.example.server_load_android.MainActivity
import kotlinx.coroutines.CoroutineStart
import kotlinx.coroutines.DelicateCoroutinesApi
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.async
import kotlinx.coroutines.awaitAll
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import java.lang.Exception
import java.net.InetSocketAddress
import java.net.Socket
import java.nio.ByteBuffer
import kotlin.concurrent.thread


@OptIn(DelicateCoroutinesApi::class)
@Composable
fun InstanceScreen() {
    if (Connector.AppPreferences?.getString("last_instance", "none") != "none") {
        val lastInstance =
            Connector.AppPreferences?.getString("last_instance", "none")?.split(":")!!
        GlobalScope.run {
            async {
                try {
                    val socket = Socket()
                    socket.soTimeout = 5000
                    socket.connect(
                        InetSocketAddress(lastInstance[0], lastInstance[1].toInt()),
                        5000
                    )
                    Connector.Connection = socket
                    Connector.Connection?.getInputStream()?.read(ByteArray(1024))
                    Connector.AppPreferences?.edit()
                        ?.putString("last_instance", "${lastInstance[0]}:${lastInstance[1]}")
                        ?.apply()
                    if (Connector.AppPreferences?.getString(
                            "${lastInstance[0]}:${lastInstance[1]}",
                            "none"
                        ) == "none"
                    ) {
                        Connector.Connection?.getOutputStream()?.write("register".toByteArray())
                        val buff = ByteArray(1024)
                        val len = Connector.Connection?.getInputStream()?.read(buff)
                        val res = String(buff, 0, len!!).split(" ")
                        if (res.count() == 2 && res[0] == "registered") {
                            Connector.AppPreferences?.edit()
                                ?.putString("${lastInstance[0]}:${lastInstance[1]}", res[1])
                                ?.apply()
                        } else {
                            throw Exception("Can't register $res")
                        }
                    } else {
                        Connector.Connection?.getOutputStream()?.write(
                            "login ${
                                Connector.AppPreferences?.getString(
                                    "${lastInstance[0]}:${lastInstance[1]}",
                                    "none"
                                )
                            }".toByteArray()
                        )
                        val buff = ByteArray(1024)
                        val len = Connector.Connection?.getInputStream()?.read(buff)
                        val res = String(buff, 0, len!!)
                        if (res != "logged") {
                            Connector.Connection?.getOutputStream()?.write("register".toByteArray())
                            val buff = ByteArray(1024)
                            val len = Connector.Connection?.getInputStream()?.read(buff)
                            val res = String(buff, 0, len!!).split(" ")
                            if (res.count() == 2 && res[0] == "registered") {
                                Connector.AppPreferences?.edit()
                                    ?.putString("${lastInstance[0]}:${lastInstance[1]}", res[1])
                                    ?.apply()
                            } else {
                                throw Exception("Can't register $res")
                            }
                        }
                    }
                    Connector.AppMainThread.post {
                        Connector.NavController?.navigate("Priority")
                        Connector.BottomNavBarVisibility.floatValue = 1f
                    }
                    GlobalScope.run { 
                        async { 
                            GameState.requestResolver.invoke()
                        }
                    }
                    return@async
                } catch (e: Exception) {
                    Connector.AppMainThread.post {
                        AlertDialog.Builder(Connector.NavController!!.context).setTitle("Error")
                            .setMessage("${e.message.toString()}\n${e.javaClass}\n${e.stackTrace[0]}")
                            .show()
                        Connector.AppPreferences?.edit()?.putString("last_instance", "none")
                            ?.apply()
                    }
                }
            }
        }
    }
    Connector.BottomNavBarVisibility.floatValue = 0f
    var instance_ip by remember {
        mutableStateOf("")
    }
    var instance_port by remember {
        mutableStateOf("")
    }

    Column(
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.fillMaxSize()
    ) {
        TextField(
            value = instance_ip,
            onValueChange = { instance_ip = it },
            placeholder = { Text(text = "Instance IP address") })
        TextField(
            value = instance_port,
            onValueChange = { instance_port = it },
            placeholder = { Text(text = "Instance listen port") })
        Button(onClick = {
            val lastInstance = "${instance_ip}:${instance_port}".split(":")
            GlobalScope.async {
                try {
                    val socket = Socket()
                    socket.soTimeout = 5000
                    socket.connect(
                        InetSocketAddress(lastInstance[0], lastInstance[1].toInt()),
                        5000
                    )
                    Connector.Connection = socket
                    Connector.Connection?.getInputStream()?.read(ByteArray(1024))
                    Connector.AppPreferences?.edit()
                        ?.putString("last_instance", "${instance_ip}:${instance_port}")?.apply()
                    if (Connector.AppPreferences?.getString(
                            "${lastInstance[0]}:${lastInstance[1]}",
                            "none"
                        ) == "none"
                    ) {
                        Connector.Connection?.getOutputStream()?.write("register".toByteArray())
                        val buff = ByteArray(1024)
                        val len = Connector.Connection?.getInputStream()?.read(buff)
                        val res = String(buff, 0, len!!).split(" ")
                        if (res.count() == 2 && res[0] == "registered") {
                            Connector.AppPreferences?.edit()
                                ?.putString("${lastInstance[0]}:${lastInstance[1]}", res[1])
                                ?.apply()
                        } else {
                            throw Exception("Can't register $res")
                        }
                    } else {
                        Connector.Connection?.getOutputStream()?.write(
                            "login ${
                                Connector.AppPreferences?.getString(
                                    "${lastInstance[0]}:${lastInstance[1]}",
                                    "none"
                                )
                            }".toByteArray()
                        )
                        val buff = ByteArray(1024)
                        val len = Connector.Connection?.getInputStream()?.read(buff)
                        val res = String(buff, 0, len!!)
                        if (res != "logged") {
                            Connector.Connection?.getOutputStream()
                                ?.write("register".toByteArray())
                            val buff = ByteArray(1024)
                            val len = Connector.Connection?.getInputStream()?.read(buff)
                            val res = String(buff, 0, len!!).split(" ")
                            if (res.count() == 2 && res[0] == "registered") {
                                Connector.AppPreferences?.edit()
                                    ?.putString("${lastInstance[0]}:${lastInstance[1]}", res[1])
                                    ?.apply()
                            } else {
                                throw Exception("Can't register $res")
                            }
                        }
                    }
                    Connector.AppMainThread.post {
                        Connector.NavController?.navigate("Priority")
                        Connector.BottomNavBarVisibility.floatValue = 1f
                    }
                    GlobalScope.run {
                        async {
                            GameState.requestResolver.invoke()
                        }
                    }
                } catch (e: Exception) {
                    Connector.AppMainThread.post {
                        AlertDialog.Builder(Connector.NavController!!.context).setTitle("Error")
                            .setMessage("${e.message.toString()}\n${e.javaClass}\n${e.stackTrace[0]}")
                            .show()
                        Connector.AppPreferences?.edit()?.putString("last_instance", "none")
                            ?.apply()
                    }
                }
            }


        }) {
            Text(text = "Enter")
        }
    }
}