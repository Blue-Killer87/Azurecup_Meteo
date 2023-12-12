# Azurecup - Meteo - 2022

This is a repository containing all the code from a project that claimed 2nd place at Azurecup 2022. 

Our project was focused on measuring quaility of air in working environment, mostly focusing on industry but it could be also used for personal usage.
Aside from hardware we've connected together it was also important to bring the whole construct together via connecting it to Azure cloud, creating a simple yet working control environment to fulfill all the needs a user may have. Connecting hardware such as ESP2886 to a cloud such as Azure wasn't that easy task, but it was not impossible and with few hundreds lines of code it became reality.
With working connection to Azure IoT Central, we added few sensor such as BME280 and SPG30. Now we had collected data and we created a way to transfer them into the cloud.
Processing of the data was mainly on Azure itself, we made a simple Json string that was being sent to the IoT central directly, where it was processed and stored into a local database. Aside from simple storing of the data we also made sure to add some fancy graphs and charts making a nice looking dashboard. 
One of the other features were also AI, controlling the validity of the data that was being presented to the database and Power Automate to make our alerts more smooth and connected with other services such as Microsoft Teams.
