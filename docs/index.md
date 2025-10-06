---
layout: default
title: Home
---

# bpm-finder

A C++ application for BPM detection.

## Documentation

### App

- [Overview](app/overview.md) - Application architecture and structure

### Core

- [Copy Observer Pattern]
- [Copy Sink](core/copy-sink.md) - Base class for data sinks that copy data from the dsp chain and then process it
  further

## Recent Posts

{% for post in site.posts limit:5 %}

- [{{ post.title }}]({{ post.url | relative_url }}) - {{ post.date | date: "%B %d, %Y" }}
  {% endfor %}
