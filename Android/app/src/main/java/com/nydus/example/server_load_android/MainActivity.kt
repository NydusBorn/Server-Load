package com.nydus.example.server_load_android

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.BottomNavigation
import androidx.compose.material.BottomNavigationItem
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.TextUnitType
import androidx.navigation.NavController
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.currentBackStackEntryAsState
import androidx.navigation.compose.rememberNavController
import androidx.preference.Preference
import com.nydus.example.server_load_android.ui.theme.ServerloadandroidTheme
import com.nydus.example.server_load_android.Screens.BuildingScreen
import com.nydus.example.server_load_android.Screens.InstanceScreen
import com.nydus.example.server_load_android.Screens.PriorityScreen
import com.nydus.example.server_load_android.Screens.ResearchScreen
import com.nydus.example.server_load_android.Screens.SettingScreen

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Connector.AppPreferences = this.getSharedPreferences("server_load_android_preferences", MODE_PRIVATE)
        setContent {
            ServerloadandroidTheme {
                // A surface container using the 'background' color from the theme
                val navController = rememberNavController()
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    Scaffold(bottomBar = {BottomNavBar(navController = navController)}){paddingValues -> NavHostContainer(
                        navController = navController,
                        paddingValues = paddingValues
                    ) }
                }
            }
        }
    }
}

@Composable
fun NavHostContainer(
    navController: NavHostController,
    paddingValues: PaddingValues
) {
    NavHost(navController = navController, startDestination = "Instance", modifier = Modifier.padding(paddingValues)) {
        composable("Instance"){
            InstanceScreen()
        }
        composable("Priority") {
            PriorityScreen()
        }
        composable("Building") {
            BuildingScreen()
        }
        composable("Research") {
            ResearchScreen()
        }
        composable("Setting") {
            SettingScreen()
        }
    }
}

@Composable
fun BottomNavBar(navController: NavController) {
    Connector.NavController = navController
    BottomNavigation (backgroundColor = MaterialTheme.colorScheme.primary, modifier = Modifier.alpha(Connector.BottomNavBarVisibility.floatValue)){
        val navBackStackEntry by navController.currentBackStackEntryAsState()
        val currentRoute = navBackStackEntry?.destination?.route
        Constants.BottomNavItems.forEach{
            BottomNavigationItem(
                selected = currentRoute == it.route, 
                onClick = { navController.navigate(it.route) }, 
                icon = { it.icon },
                label = { Text(text = it.label, fontSize = TextUnit(3f, TextUnitType.Em)) },
                alwaysShowLabel = false,
                enabled = Connector.BottomNavBarVisibility.floatValue != 0f
            )
        }
    }
}