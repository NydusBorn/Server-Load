package com.nydus.example.server_load_android.Screens

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.TextField
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.modifier.modifierLocalMapOf
import com.nydus.example.server_load_android.Connector
import com.nydus.example.server_load_android.MainActivity


@Composable
fun InstanceScreen (){
    Connector.BottomNavBarVisibility.floatValue = 0f
    Column(verticalArrangement = Arrangement.Center, horizontalAlignment = Alignment.CenterHorizontally, modifier = Modifier.fillMaxSize()) {
        TextField(value = "", onValueChange = {}, placeholder = { Text(text = "Instance IP address")})
        TextField(value = "", onValueChange = {}, placeholder = { Text(text = "Instance listen port")})
        Button(onClick = {
            Connector.navHost?.navigate("Building")
            Connector.BottomNavBarVisibility.floatValue = 1f
        }) {
            Text(text = "Enter")
        }
    }
}