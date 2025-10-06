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

- [Sink](core/sink.md) - Base class for DSP chain consumers

## Recent Posts

{% for post in site.posts limit:5 %}

- [{{ post.title }}]({{ post.url | relative_url }}) - {{ post.date | date: "%B %d, %Y" }}
  {% endfor %}
