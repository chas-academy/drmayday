# Dr. Mayday - WIP

## Getting started

```
git clone git@github.com:chas-academy/drmayday.git
cd drmayday
npm install
```

See `.env.template` for env-variables.

To start live-compiling ReasonML to Javascript run:
`npm start`
then in another terminal run:
`npm run dev`
to fire up the Express-server.

### DB

```
CREATE TABLE `help_items` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` varchar(255) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `time_created` double DEFAULT NULL,
  `time_closed` double DEFAULT NULL,
  `finished` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
```

## How To Use

## Available commands

| Command                | Privilegies | Description                   |
| ---------------------- | ----------- | ----------------------------- |
| `help`                 | `*`         | Display the help text         |
| `mayday <Description>` | `Student`   | Add yourself to the help list |
| `next`                 | `Teacher`   | Mark help-item as finished    |
| `queue`                | `*`         | Show the current queue        |

## Standards

- [Semantic commits](https://gist.github.com/lessp/7af7ca9e3a2f3508755dcddfcbc1f15d)

## ... more to come
