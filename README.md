# Dr. Mayday - WIP

## Table of Contents

1. [Getting started](#getting-started)
   - [DB](#db)
   - [Slack](#slack)
2. [Usage](#usage)
3. [Standards](#standards)
4. [Misc](#misc)
   - [Changelog and tagging](#changelog-and-tagging)
   - [Credits](#credits)

## [Getting started](#getting-started)

```
git clone git@github.com:chas-academy/drmayday.git
cd drmayday
npm install
```

See `.env.template` for env-variables.

- Slack token is found under `Features -> OAuth & Permissions -> Bot User OAuth Access Token`

To start live-compiling ReasonML to Javascript run:
`npm start`
then in another terminal run:
`npm run dev`
to fire up the Express-server.

### [DB](#db)

```
CREATE TABLE `help_items` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` varchar(255) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `room` varchar(255) DEFAULT NULL,
  `time_created` double DEFAULT NULL,
  `time_closed` double DEFAULT NULL,
  `finished` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
```

### [Slack](#slack)

The easiest way to get up and running is to use ngrok pointed against the dev-server (eg. `ngrok http 4000`)

You'll need to change the host-adress at the following locations in the Slack API.

- actions: `Features -> Interactive Components`
- event: `Features -> Event Subscriptions`
- mayday (/mayday): `Features -> Slash Commands`

## [How To Use](#usage)

## Available commands

| Command                | Short | Privileges | Description                   |
| ---------------------- | ----- | ---------- | ----------------------------- |
| `clear`                |       | `Teacher`  | Mark all items as finished    |
| `help`                 |       | `*`        | Display the help text         |
| `mayday <Description>` | `m`   | `Student`  | Add yourself to the help list |
| `next`                 |       | `Teacher`  | Mark help-item as finished    |
| `queue`                | `q`   | `*`        | Show the current queue        |

## [Standard](#standards)

- [Semantic commits](https://gist.github.com/lessp/7af7ca9e3a2f3508755dcddfcbc1f15d) 
- [Standard Version](https://github.com/conventional-changelog/standard-version)

## [Misc](#misc)

### [Changelog and tagging](#changelog-and-tagging)

To update the changelog and tag a new release we use [Standard Version](https://github.com/conventional-changelog/standard-version).

**Follow these steps:**

```
git checkout master; git pull origin master
npm run release
npm run tag // just a script to run "git push --follow-tags origin master"
```

### [Credits](#credits)

**Icon used in Slack:**

Icons made by [Freepik](https://www.freepik.com) from [www.flaticon.com](https://www.flaticon.com/) is licensed by [CC 3.0 BY](http://creativecommons.org/licenses/by/3.0/)
