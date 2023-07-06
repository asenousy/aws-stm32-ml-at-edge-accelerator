# AWS STM32 ML at Edge Accelerator

This is an AWS STM32 example project that implements MLOps infrastructure using SageMaker pipeline to train and generate an audio classification model that will run on edge devices (stm32u5 series) with OTA updates using Amazon Freertos. Devices are connected to Iot Core and data is collected via mqtt.

## Architecture

![architecture](./doc/images/architecture.png)

## Deployment

The project is built using CDK IaC. So it can be deployed to your AWS account with a single deploy command.

### Pre-requisites

- Ensure your AWS credentials are in place for your Pipeline account (The account that will host the pipeline stack if different)
- Ensure you have [Node.js](https://nodejs.org) and [Docker](https://www.docker.com/products/docker-desktop/) installed
- Create an account in [STM32Cube.AI Developer Cloud](https://stm32ai-cs.st.com/home)
- In your AWS Account that will host the ML Stack (if different). Go to secrets manager console page:
  1.  Create a new secret for your ST username
      1.  Choose type other
      1.  Choose Plaintext tab
      1.  Overwrite content with you username
      1.  Give this secret the following name `ST_DEVCLOUD_USERNAME_SECRET` (if you require a different name, then ensure to update this new name in `cdk.json`)
  1.  Create a new secret for your ST password
      1.  Repeat the previous step but with the following secret name `ST_DEVCLOUD_PASSWORD_SECRET`
          run the following commands
- Bootstrap your account (each account if more than one) with the following command replacing your account and region placeholders
  ```
  npx cdk bootstrap aws://<ACCOUNT-NUMBER>/<REGION> --toolkit-stack-name CDKToolkit-StMicro --qualifier stmicro
  ```

### deploy the CICD Pipeline

1. Fork this repo.
1. Clone your fork locally.
1. Go to [cdk.json](./cdk.json) and put your aws account details for each stack in the `env` field, you could deploy the stacks in the same account or in different accounts
1. You need to set up a connection between the pipeline and your forked Repo
   1. Follow the steps to create & verify the connection [here](https://docs.aws.amazon.com/dtconsole/latest/userguide/connections-create-github.html)
   1. Go to [cdk.json](./cdk.json) and paste your forked Repo name, branch & connection Arn in the config section.
1. In the root folder, run the following commands
   ```
   npm install
   npm run cdk synth
   ```
1. You will notice a `cdk.context.json` is created. Please commit this file along with your edited `cdk.json`. And push the commit to your forked repo.
1. Once your repo has been updated with the `cdk.json` & the newly created `cdk.context.json`, we can finally deploy with
   ```
   npm run deploy
   ```
1. This is the only time you need to run commands locally, for any future changes just push new commits to your repo and the pipeline redeploy the new code changes.

### Setup the Device

These steps assume you have a [B-U585I-IOT02A](https://www.arrow.com/en/products/b-u585i-iot02a/stmicroelectronics) device

#### Pre-requisites

- Ensure your AWS credentials are in place for your IoT account (The account that will host the IoT stack if different)
- Download and Install [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) to be able to flash device with initial firmware

  **Note**
  if installing on a Mac make sure you run the following command before installing to give it exec permission `sudo xattr -cr ~/SetupSTM32CubeProgrammer-macos.app`

#### Provision Your Board

To provision your board follow use scripts by following the below steps:

setup your environment

```
sh tools/env_setup.sh
```

then source your environment

```
source .venv/bin/activate
```

In your AWS Console go to the cloudformation page and then click on IoTStack and view the Output tab. Copy and run the `ProvisionScript` replacing the <> with a unique name for your device

the script should look like below

```
python tools/provision.py --interactive  --thing-name <Thing-Name>
```

After you provision the device successfully. Go back to the output tab of the IotStack in cloudformation page. And copy the public key value.

**Warning**
The displayed public key might not be properly formatted, please ensure you fix the format using any text editor to add the correct newlines as seen below before continuing

```
-----BEGIN PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAECSEgXkzVQkq2fQwPjIhKIkvJBVCl
VaRDhiyj9BGkHbuAnGzM0gC+z6oYpxbkgE3qa6fHJoE99QTwrRh8XWwyCg==
-----END PUBLIC KEY-----
```

And follow below steps:

1. connect to device using `screen /dev/tty.usbmodem103 115200`
1. run this command `pki import key ota_signer_pub`
1. paste the public key you copied. you should a confirmation of key being registered

## Flashing the device

Before we can use over the air (OTA) firmware updates, we need to atlease flash the device with the firmware manually once. Then future changes will be handled by Freertos OTA updates.

After deployment is successfull a binary firmware file should have been created in s3 bucket. You can find the name of the bucket in the same cloudformation output tab for the iotStack. The binary file will have the follwing name `b_u585i_iot02a_ntz.bin`, please download this file.
If you have already installed [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html), follow these steps to flash the device with the firware.

1. Connect your device to your computer
1. Open STM32CubeProgrammer
1. Click Connect button on top right to connect to device
1. Go to Menu and choose `Erase & programming`
1. Click Browse and choose our binary file we just downloaded from s3
1. Click `Start Programming` to flash binary to device
1. When finished click `Disconnect` on top right corner

**Note**
if installing on a Mac make sure you run the following command before installing to give it exec permission `sudo xattr -cr ~/SetupSTM32CubeProgrammer-macos.app`

Once flashed if you are still connected to the device using the command `screen /dev/tty.usbmodem103 115200`, you should see the device starting to log sound class detected from sensors.

## Iot

Our device should be publishing MQTT messages now. If you go to AWS IoT console page and open MQTT test client, type the device name in the filter and subscribe. You will see the messages coming in.

These messages are streamed and stored into AWS Iot Analytics. Which will then be used as a datasource for Quicksight to visualise it.

## Quicksight Dashboard

To visualise your data, we will be using Quicksight. Go to quicksight console page and you will need to sign up to use Quicksight. While going through the steps when you come across the section titled `Allow access and autodiscovery for these resources` make sure you enable `AWS IoT Analytics`. Once you are inside Quicksight, just do the following:

1. Click New Analysis
1. Click New dataset
1. Choose AWS IoT Analytics
1. Select our Analytics dataset and click Create data source
1. Click Visualize
1. Choose the Free-form layout and click Create

you should be able to display a graph as image below.

![quicksight](./doc/images/quicksight.png)

## Clean up

To remove all resources created by this stack run the following

```
npm run destroy
```

## Security

See [CONTRIBUTING](CONTRIBUTING.md#security-issue-notifications) for more information.

## License

This library is licensed under the MIT-0 License. See the LICENSE file.
