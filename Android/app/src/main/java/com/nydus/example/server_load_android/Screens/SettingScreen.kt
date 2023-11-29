package com.nydus.example.server_load_android.Screens

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalLifecycleOwner
import com.nydus.example.server_load_android.Connector
import com.nydus.example.server_load_android.GameState
import com.nydus.example.server_load_android.ServerRequest
import com.nydus.example.server_load_android.ServerRequestType
import java.time.Instant

@Composable
fun SettingScreen() {
    LaunchedEffect(LocalLifecycleOwner.current){
        GameState.requestQueue.add(ServerRequest(ServerRequestType.update, Instant.now(), GameState::update))
    }
    Column(
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.fillMaxSize()
    ) {
        Button(onClick = {
            Connector.Connection?.close()
            Connector.AppPreferences?.edit()?.putString("${Connector.AppPreferences?.getString("last_instance", "none")}", "none")?.apply()
            Connector.NavController?.navigate("Instance")
        }) {
            Text(text = "Reset Progress on instance")
        }
        Button(onClick = { 
            Connector.Connection?.close()
            Connector.AppPreferences?.edit()?.putString("last_instance", "none")?.apply()
            Connector.NavController?.navigate("Instance")
        }) {
            Text(text = "Choose another instance")
        }
    }
}